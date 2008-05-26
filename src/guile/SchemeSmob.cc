/*
 * SchemeSmob.c
 *
 * Scheme small objects.
 * Copyright (c) 2008 Linas Vepstas <linas@linas.org>
 */

#ifdef HAVE_GUILE

#include <libguile.h>
#include <vector>

#include "Atom.h"
#include "ClassServer.h"
#include "CogServer.h"
#include "Link.h"
#include "Node.h"
#include "SchemeSmob.h"
#include "TLB.h"

using namespace opencog;

/* ============================================================== */

bool SchemeSmob::is_inited = false;
scm_t_bits SchemeSmob::cog_tag;

SchemeSmob::SchemeSmob(void)
{
	if (!is_inited)
	{
		is_inited = true;
		init_smob_type();
		register_procs();
	}
}

/* ============================================================== */

int SchemeSmob::print_cog(SCM node, SCM port, scm_print_state * ps)
{
	scm_puts ("#<atom>", port);
	return 1; //non-zero meanss success
}

SCM SchemeSmob::equalp_cog(SCM a, SCM b)
{
	printf("duuude! equalp\n");
	return SCM_BOOL_F;
}

void SchemeSmob::init_smob_type(void)
{
	cog_tag = scm_make_smob_type ("opencog_atom", sizeof (scm_t_bits));
	scm_set_smob_print (cog_tag, print_cog);
	scm_set_smob_equalp (cog_tag, equalp_cog);
}

/**
 * return atom->toString() for the corresponding atom.
 */
std::string SchemeSmob::to_string(SCM node)
{
	SCM shandle = SCM_SMOB_OBJECT(node);
	Handle h = scm_to_ulong(shandle);

	if (UNDEFINED_HANDLE == h) return "Undefined atom handle";

	if (h < NOTYPE) return "non-real atom";

	Atom *atom = TLB::getAtom(h);
	if (NULL == atom) return "Invalid handle";

	return atom->toString();
}

/* ============================================================== */
/**
 * Create a new scheme object, holding the atom handle
 */
SCM SchemeSmob::ss_atom (SCM shandle)
{
	if (scm_is_false(scm_integer_p(shandle)))
		scm_wrong_type_arg_msg("cog-atom", 1, shandle, "integer opencog handle");
	SCM_RETURN_NEWSMOB (cog_tag, shandle);
}

/* ============================================================== */
/**
 * Return handle of atom
 */
SCM SchemeSmob::ss_handle (SCM satom)
{
	if (!SCM_SMOB_PREDICATE(SchemeSmob::cog_tag, satom))
		scm_wrong_type_arg_msg("cog-handle", 1, satom, "opencog atom");
	return SCM_SMOB_OBJECT(satom);
}

/* ============================================================== */
/**
 * Create a new node, of named type stype, and string name sname
 */
SCM SchemeSmob::ss_new_node (SCM stype, SCM sname)
{
	if (scm_is_true(scm_symbol_p(stype)))
		stype = scm_symbol_to_string(stype);

   char * ct = scm_to_locale_string(stype);
	Type t = ClassServer::getType(ct);
	free(ct);

	// Make sure that the type is good
	if (NOTYPE == t)
		scm_wrong_type_arg_msg("cog-new-node", 1, stype, "name of opencog atom type");
		
	if (false == ClassServer::isAssignableFrom(NODE, t))
		scm_wrong_type_arg_msg("cog-new-node", 1, stype, "name of opencog node type");

	if (scm_is_false(scm_string_p(sname)))
		scm_wrong_type_arg_msg("cog-new-node", 2, sname, "string name for the node");

	// Now, create the actual node... in the actual atom space.
	char * cname = scm_to_locale_string(sname);
	std::string name = cname;
	free(cname);

	AtomSpace *as = CogServer::getAtomSpace();
	Handle h = as->addNode(t, name);

	SCM shandle = scm_from_ulong(h);

	SCM smob;
	SCM_NEWSMOB (smob, cog_tag, shandle);
	return smob;
}

/* ============================================================== */
/**
 * Create a new link, of named type stype, holding the indicated atom list
 */
SCM SchemeSmob::ss_new_link (SCM stype, SCM satom_list)
{
	if (scm_is_true(scm_symbol_p(stype)))
		stype = scm_symbol_to_string(stype);

   char * ct = scm_to_locale_string(stype);
	Type t = ClassServer::getType(ct);
	free(ct);

	// Make sure that the type is good
	if (NOTYPE == t)
		scm_wrong_type_arg_msg("cog-new-link", 1, stype, "name of opencog atom type");
		
	if (false == ClassServer::isAssignableFrom(LINK, t))
		scm_wrong_type_arg_msg("cog-new-link", 1, stype, "name of opencog link type");

	// Verify that second arg is an actual list
	if (!scm_is_pair(satom_list))
		scm_wrong_type_arg_msg("cog-new-link", 2, satom_list, "a list of atoms");

	std::vector<Handle> outgoing_set;
	SCM sl = satom_list;
	int pos = 2;
	do
	{
		SCM satom = SCM_CAR(sl);

		// Verify that the contents of the list are actual atoms.
		if (!SCM_SMOB_PREDICATE(SchemeSmob::cog_tag, satom))
			scm_wrong_type_arg_msg("cog-new-link", pos, satom, "opencog atom");

		// Get the handle  ... should we check for valid handles here? 
		SCM shandle = SCM_SMOB_OBJECT(satom);
		Handle h = scm_to_ulong(shandle);

		outgoing_set.push_back(h);
		sl = SCM_CDR(sl);
		pos++;
	}
	while (scm_is_pair(sl));
	
	AtomSpace *as = CogServer::getAtomSpace();

	// Now, create the actual link... in the actual atom space.
	Handle h = as->addLink(t, outgoing_set);

	SCM shandle = scm_from_ulong(h);

	SCM smob;
	SCM_NEWSMOB (smob, cog_tag, shandle);
	return smob;
}

#define C(X) ((SCM (*) ()) X)

void SchemeSmob::register_procs(void)
{
	scm_c_define_gsubr("cog-new-link",            2, 0, 0, C(ss_new_link));
	scm_c_define_gsubr("cog-new-node",            2, 0, 0, C(ss_new_node));
	scm_c_define_gsubr("cog-atom",                1, 0, 0, C(ss_atom));
	scm_c_define_gsubr("cog-handle",              1, 0, 0, C(ss_handle));
}

#endif
/* ===================== END OF FILE ============================ */
