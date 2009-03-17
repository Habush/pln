#ifndef AGENT_MODE_HANDLER_H
#define AGENT_MODE_HANDLER_H

#include <map>
#include <vector>
#include <sstream>

namespace Control {

  /**
   * Interface responsible for handle all the specific tasks of an agent action mode
   */
  class AgentModeHandler {
  public:
    
    virtual inline ~AgentModeHandler( void ) { }
    
    /**
     * It must be implemented by contrete classes
     * This method shall handle the commands given by the owner or other parts of the Brain code
     *
     * @param name the name of the command
     * @param arguments the list of command' arguments
     */  
    virtual void handleCommand( const std::string& name, const std::vector<std::string>& arguments ) = 0;

    /**
     * Returns a string containing the name of the mode
     *
     * @return string containing the mode name
     */
    virtual const std::string& getModeName( void ) = 0;

    virtual void update( void ) = 0;

    /**
     * Set a property value
     *
     * @param name Property name
     * @param value Property value
     */
    inline void setProperty( const std::string& name, const std::string& value ) {
      this->properties[name] = value; 
    }

    /**
     * Get a property value
     *
     * @param name Property name
     * @return the Property value
     */
    inline std::string getPropertyValue( const std::string& name ) {
      std::map<std::string, std::string>::iterator it = this->properties.find( name );
      if ( it != this->properties.end( ) ) {
	return it->second;
      } else {
	return "";
      } // else
    }

    /**
     * Remove a given property from the properties list
     *
     * @param name Property name
     */
    void removeProperty( const std::string& name ) {
      std::map<std::string, std::string>::iterator it = this->properties.find( name );
      this->properties.erase( it );
    }

  protected:
    std::map<std::string, std::string> properties;
  };
  
}; // Control

#endif // AGENT_MODE_HANDLER_H
