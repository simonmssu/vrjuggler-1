/* Generated by Together */

#ifndef AJSOUNDFACTORY_H
#define AJSOUNDFACTORY_H
#include <iostream>
#include <string>
#include "aj/Singleton.h"
#include "aj/SoundImplementation.h"

namespace aj
{

class SoundFactory : public aj::Singleton<SoundFactory>
{
public:

   /**
    * @input name of api to create
    * @output an implementation is returned for you to use (new memory, you delete)
    * @postconditions if apiName is not known, then a stub implementation is returned
    * @semantics factory function used to create an implementation of a sound API 
    */
   void createImplementation( const std::string& apiName,
                              aj::SoundImplementation* &mImplementation );

   // pass valid pointer to a sound implementation with a name it's refered to
   // or... pass NULL to unregister/delete apiName...
   void reg( const std::string& apiName, aj::SoundImplementation* impl )
   {
      if (impl != NULL)
      {
         impl->setName( apiName );
         mRegisteredImplementations[apiName] = impl;
         std::cout<<"Loading sound API: "<<apiName<<" ["<<(int)impl<<"]\n"<<std::flush;
      }
      else
      {
         std::cout<<"Removed sound API: "<<apiName<<" ["<<(int)impl<<"]\n"<<std::flush;
         mRegisteredImplementations.erase( apiName );
      }      
      
   }
   
   std::map< std::string, aj::SoundImplementation* > mRegisteredImplementations;
private:  
   /** @link dependency */
   /*#  aj::SoundImplementation lnkSoundImplementation; */

   /** @link dependency */
   /*#  aj::StubSoundImplementation lnkaj::StubSoundImplementation; */

   /** @link dependency */
   /*#  aj::AudioWorksSoundImplementation lnkaj::AudioWorksSoundImplementation; */

   /** @link dependency */
   /*#  aj::OpenALSoundImplementation lnkaj::OpenALSoundImplementation; */
};

template <class _type>
class SoundFactoryReg
{
public:
   _type mSoundAPI;
   std::string mNameOfAPI;
   SoundFactoryReg( const std::string& apiName ) : mNameOfAPI( apiName ), mSoundAPI()
   {
      SoundFactory::instance().reg( mNameOfAPI, &mSoundAPI );
   }
   virtual ~SoundFactoryReg()
   {
      SoundFactory::instance().reg( mNameOfAPI, NULL );
   }   
};

}; // end namespace

#endif //AJSOUNDFACTORY_H
