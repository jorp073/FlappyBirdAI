/*

Templates of infinite state machine

Reference: http://www.ai-junkie.com/architecture/state_driven/tut_state3.html

*/

#pragma once

#include <assert.h>

template <class entity_type>
class TState
{
public :
    TState(const std::string name) : m_Name(name) {};
    virtual void Enter( entity_type *) {};

    virtual void Exit( entity_type *) {};

    virtual bool Update( entity_type * ) { return false; };

    const std::string& GetName() const { return m_Name; };

protected:
    std::string m_Name;
};


template <class entity_type>
class TStateMachine
{
public :

    TStateMachine(entity_type * owner ): m_pOwner( owner ),
        m_pCurrentState ( NULL)
    {}


    void InitState( TState< entity_type >* s )
    {
        printf("InitState to %s\n", s->GetName().c_str());
        m_pCurrentState = s ;
        s->Enter ( m_pOwner);
    }


    //use these methods to initialize the FSM
    void SetCurrentState( TState< entity_type >* s ) {m_pCurrentState = s;}


    //call this to update the FSM
    bool Update()
    {
        //same for the current state
        if ( m_pCurrentState )
            return m_pCurrentState -> Update( m_pOwner );
        else
            return false;
    }


    //change to a new state
    void ChangeState( TState< entity_type >* pNewState )
    {
        assert (pNewState &&
            "<StateMachine::ChangeState>: trying to change to a null state" );
        printf("ChangeState from: %s to %s\n", m_pCurrentState->GetName().c_str(), pNewState->GetName().c_str());

        //call the exit method of the existing state
        m_pCurrentState -> Exit( m_pOwner );

        //change state to the new state
        m_pCurrentState = pNewState ;

        //call the entry method of the new state
        m_pCurrentState -> Enter( m_pOwner );
    }


    //accessors
    TState <entity_type >* CurrentState() const { return m_pCurrentState ;}


    //returns true if the current state¡¯s type is equal to the type of the
    //class passed as a parameter.
    bool IsInState(std::string name) const
    {
        return m_pCurrentState->GetName() == name;
    }


private :
    //a pointer to the agent that owns this instance
    entity_type *          m_pOwner ;

    TState <entity_type >*    m_pCurrentState ;
};
