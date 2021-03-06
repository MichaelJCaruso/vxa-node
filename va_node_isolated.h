#ifndef VA_Node_Isolated_Interface
#define VA_Node_Isolated_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

#include "va_node_isolate.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Isolated : public Entity {
            DECLARE_ABSTRACT_RTTLITE (Isolated, Entity);

        //  Class Builder
        public:
            class ClassBuilder
                : public BaseClass::ClassBuilder
            {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };
            friend class ClassBuilder;

        //  Construction
        protected:
            Isolated (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Isolated ();

        //  Decommissioning
/************************************************************************************
 *  An 'Isolated's destructor may be called from any thread.  As of this writing,
 *  it is not safe to destroy 'node', 'v8' or 'uv' objects (e.g., 'Reset' a
 *  'v8::Persistent', 'uv_close' a 'uv_async_t' handle, etc.) on any thread other
 *  than the node/uv main thread.  While these resources are not currently subject
 *  to automatic destruction ("uv.h" structs are C objects, 'v8::Persistent' handles
 *  are not automatically reset on destruction (see the comments assocated with
 *  'kResetInDestructor' in "v8.h"), the apparent quiet 'success' of destructors
 *  that should be releasing resources comes at the cost of leaked memory.  To
 *  address these issues, this class provides support for a two phase destruction
 *  process (* SEE BELOW *) that arranges for the reclamation of resources in the
 *  correct thread. To take advantage of that process, every derived class with
 *  resources to reclaim must define a 'protected' ('private' is ok for 'final'
 *  classes) virtual override of this class' 'decommission' member that releases
 *  the resources allocated by this class and calls the base class version of
 *  'decommission':
 *  
 *    bool VA::Node::DerivedClass::decommission () {
 *    //  Free the resources held by this instance ...
 *        m_hSomething.Reset ();
 *        somethingMoreComplicated ();
 *        etc ();
 *
 *    //  Call the base class' version of 'decommission':
 *        return BaseClass::decommission ();
 *    }
 *
 *--------------------------------------------------------------------------------
 *
 *  (*) IMPORTANT IMPLEMENTATION NOTE (*)
 *
 *  The decommissioning process is implemented using the 'onDeleteThis' hook provided
 *  by the object lifetime management infrastructure inherited from 'Isolated's base
 *  class(es).  That hook gives derived classes an opportunity to approve an object's
 *  destruction (by returning 'true') or to defer or cancel that destruction (by
 *  returning 'false' after presumably making alternative arrangements) as is done
 *  here:
 *
 *    bool VA::Node::Isolated::onDeleteThis () {
 *        return m_pIsolate->okToDecommission (this) && decommission ();
 *    }
 *
 ************************************************************************************/
        protected:
            bool onDeleteThis ();
            virtual bool decommission ();

        //  Access
        public:
            Isolate::call_count_t callCount () const {
                return m_pIsolate->callCount ();
            }

            Isolate *isolate () const {
                return m_pIsolate;
            }
            v8::Isolate *isolateHandle () const {
                return m_pIsolate->handle ();
            }
            local_context_t context () const {
                return m_pIsolate->context ();
            }

        //  Model Management
        public:
            template <typename handle_t> bool Attach (
                ClassTraits<Export>::retaining_ptr_t &rpModelObject, handle_t hValue
            ) const {
                return m_pIsolate->Attach (rpModelObject, hValue);
            }

        //  Local Access
        public:
        //  ... isolate constants
            local_primitive_t LocalUndefined () const {
                return m_pIsolate->LocalUndefined ();
            }
            local_primitive_t LocalNull () const {
                return m_pIsolate->LocalNull ();
            }
            local_boolean_t LocalTrue () const {
                return m_pIsolate->LocalTrue ();;
            }
            local_boolean_t LocalFalse () const {
                return m_pIsolate->LocalFalse ();
            }
            bool GetLocalUndefined (local_value_t &rhLocal) const {
                return m_pIsolate->GetLocalUndefined (rhLocal);
            }
            bool GetLocalNull (local_value_t &rhLocal) const {
                return m_pIsolate->GetLocalNull (rhLocal);
            }
            bool GetLocalTrue (local_value_t &rhLocal) const {
                return m_pIsolate->GetLocalTrue (rhLocal);
            }
            bool GetLocalFalse (local_value_t &rhLocal) const {
                return m_pIsolate->GetLocalFalse (rhLocal);
            }

        public:
        //  ... handle -> local handle
            template <typename source_t> typename V8<source_t>::local LocalFor (
                source_t const &rHandle
            ) const {
                return m_pIsolate->LocalFor (rHandle);
            }

        //  ... handle -> local handle (maybe)
            template <typename local_t, typename source_t> bool GetLocalFrom (
                local_t &rhLocal, source_t const &rhSource
            ) const {
                return m_pIsolate->GetLocalFrom (rhLocal, rhSource);
            }

        //  Object Access
        public:
            template <typename unwrapped_t, typename handle_t> bool GetUnwrapped (
                unwrapped_t &rUnwrapped, handle_t hValue
            ) const {
                return m_pIsolate->GetUnwrapped (rUnwrapped, hValue);
            }
            template <typename handle_t> bool UnwrapString (
                VString &rString, handle_t hString, bool bDetail = false
            ) const {
                return m_pIsolate->UnwrapString (rString, hString, bDetail);
            }

        //  Object Creation
        public:
            local_string_t NewString (char const *pString) const {
                return m_pIsolate->NewString (pString);
            }
            local_object_t NewObject () const {
                return m_pIsolate->NewObject ();
            }

        //  Result Return
        public:
        /*--------------------------*
         *----  Maybe Callback  ----*
         *--------------------------*/
            template<typename result_t,typename callable_t,typename... arg_ts> bool MaybeSetResultToCall (
                result_t &rResult,
                node::async_context aContext,
                local_object_t hReceiver,
                callable_t hCallable,
                arg_ts ...args
            ) const {
                return m_pIsolate->MaybeSetResultToCall (
                    rResult, aContext, hReceiver, hCallable, args...
                );
            }

            template<typename result_t,typename callable_t,typename pack_t> bool MaybeSetResultToApply (
                result_t &rResult,
                node::async_context aContext,
                local_object_t hReceiver,
                callable_t hCallable,
                pack_t &rPack
            ) const {
                return m_pIsolate->MaybeSetResultToApply (
                    rResult, aContext, hReceiver, hCallable, rPack
                );
            }

        /*----------------------------*
         *----  Maybe Call/Apply  ----*
         *----------------------------*/
            template <typename result_t, typename callable_t, typename... arg_ts> bool MaybeSetResultToCall (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, arg_ts ...args
            ) const {
                return m_pIsolate->MaybeSetResultToCall (rResult, hReceiver, hCallable, args...);
            }

            template <typename result_t, typename callable_t, typename pack_t> bool MaybeSetResultToApply (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, pack_t &rPack
            ) const {
                return m_pIsolate->MaybeSetResultToApply (rResult, hReceiver, hCallable, rPack);
            }

        /*-----------------------*
         *----  Maybe Value  ----*
         *-----------------------*/
            template <typename result_t, typename handle_t> bool MaybeSetResultToValue (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToValue (rResult, hValue);
            }
            template <typename result_t, typename handle_t> bool MaybeSetResultToInt32 (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToInt32 (rResult, hValue);
            }
            template <typename result_t, typename handle_t> bool MaybeSetResultToDouble (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToDouble (rResult, hValue);
            }
            template <typename result_t, typename handle_t> bool MaybeSetResultToString (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToString (rResult, hValue);
            }
            template <typename result_t, typename handle_t> bool MaybeSetResultToObject (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToObject (rResult, hValue);
            }

        /****************************
         *----  SetResultTo...  ----*
         ****************************/
            template <typename result_t, typename callable_t, typename... arg_ts> bool SetResultToCall (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, arg_ts ...args
            ) const {
                return m_pIsolate->MaybeSetResultToCall (rResult, hReceiver, hCallable, args...);
            }
            template <typename result_t, typename callable_t, typename pack_t> bool SetResultToApply (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, pack_t &rPack
            ) const {
                return m_pIsolate->SetResultToApply (rResult, hReceiver, hCallable, rPack);
            }
            template <typename result_t, typename handle_t> bool SetResultToValue (
                result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->SetResultToValue (rResult, hValue);
            }

            template <typename result_t> bool SetResultToGlobal (result_t &rResult) const {
                return m_pIsolate->SetResultToGlobal (rResult);
            }
            template <typename result_t> bool SetResultToUndefined (result_t &rResult) const {
                return m_pIsolate->SetResultToUndefined (rResult);
            }

        //  Task Launcher
        public:
            static bool launchTask (Vxa::VTask *pTask);

        //  State
        private:
            Isolate::Reference const m_pIsolate;
        };
    } // namespace VA::Node
} // namespace VA


#endif
