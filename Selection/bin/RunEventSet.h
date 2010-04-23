#ifndef RUNEVENTSET_H
#define RUNEVENTSET_H

// Written by Peter Dong 060504
// -- Added RunSet, Charles Plager 060512

// Adding and then checking 1 million events on a 1 GHz machine took
// 10 seconds and ~25 Mb of memory

#include <set>

class RunEventSet
{
   public:
      RunEventSet() {}

      bool alreadySeen (unsigned int run, unsigned int event);

   private:
      typedef std::pair< unsigned int, unsigned int > RunEventPair;

      std::set< RunEventPair > m_set;
};

inline bool RunEventSet::alreadySeen (unsigned int run, unsigned int event)
{
   RunEventPair myRunEvent(run, event);

   // Have we seen this pair before?
   if ( m_set.end() == m_set.find(myRunEvent) )
   {
      // This pair has not been seen before.  Put it in the set
      m_set.insert(myRunEvent);
      return false;
   }
   else
   {
      // This pair has been seen before
      return true;
   }
}

class RunSet
{
   public:
      RunSet() {}
      
      // add a run to the list to be checked
      void addRun (unsigned int run);
      // is this a run we want to check?
      bool checkRun (unsigned int run) const;

   private:
      std::set< unsigned int > m_set;      
};


inline void RunSet::addRun (unsigned int run)
{
   m_set.insert (run);
}

inline bool RunSet::checkRun (unsigned int run) const
{
   return ( m_set.end() != m_set.find (run) );
}

#endif
