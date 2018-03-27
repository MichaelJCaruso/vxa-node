#ifndef VA_Node_Isolate_Interface
#define VA_Node_Isolate_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_process.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Export;
        class Isolated;

        class Isolate : public Entity {
            DECLARE_CONCRETE_RTTLITE (Isolate, Entity);

            friend class Process::Primary;

            friend class Isolated;
            friend class Export;

        //  Aliases
        public:
            typedef v8::Isolate isolate_t;
            typedef isolate_t*  isolate_handle_t;
            typedef isolate_t*  isolate_global_t;

            typedef v8::NativeWeakMap              object_cache_t;
            typedef V8<object_cache_t>::local      object_cache_handle_t;
            typedef V8<object_cache_t>::persistent object_cache_global_t;

            typedef ClassTraits<Export>::retaining_ptr_t ExportReference;
            typedef ClassTraits<Export>::notaining_ptr_t ExportPointer;

        //  class Args
        public:
            class Args;

        //  Construction
        private:
            Isolate (v8::Isolate *pIsolate);

        //  Destruction
        private:
            ~Isolate ();

        //  Instantiation
        public:
            static bool GetInstance (Reference &rpInstance, v8::Isolate *pIsolate);

        //  Decommisioning
        protected:
            bool onDeleteThis ();                             // ... myself
            bool okToDecommision (Isolated *pIsolated) const; // ... others

        //  Access
        public:
            isolate_handle_t isolate () const {
                return m_hIsolate;
            }
            isolate_handle_t handle () const {
                return m_hIsolate;
            }
            operator isolate_handle_t () const {
                return m_hIsolate;
            }
            local_context_t context () const {
                return m_hIsolate->GetCurrentContext ();
            }

        //  Local Access
        public:
        //  ... isolate constants
            local_primitive_t LocalUndefined () const {
                return v8::Undefined (m_hIsolate);
            }
            local_primitive_t LocalNull () const {
                return v8::Null (m_hIsolate);
            }
            local_boolean_t LocalTrue () const {
                return v8::True (m_hIsolate);
            }
            local_boolean_t LocalFalse () const {
                return v8::False (m_hIsolate);
            }
            bool GetLocalUndefined (local_value_t &rhLocal) const {
                rhLocal = LocalUndefined ();
                return true;
            }
            bool GetLocalNull (local_value_t &rhLocal) const {
                rhLocal = LocalNull ();
                return true;
            }
            bool GetLocalTrue (local_value_t &rhLocal) const {
                rhLocal = LocalTrue ();
                return true;
            }
            bool GetLocalFalse (local_value_t &rhLocal) const {
                rhLocal = LocalFalse ();
                return true;
            }

        //  ... handle -> local handle
            template <typename source_t> typename V8<source_t>::local LocalFor (
                source_t const &rhSouce
            ) const {
                return V8<source_t>::local::New (m_hIsolate, rhSouce);
            }

        //  ... handle -> local handle (maybe)
            template <typename local_t> bool GetLocalFrom (
                local_t &rhLocal, persistent_value_t const &hValue
            ) const {
                return GetLocalFrom (rhLocal, LocalFor (hValue));
            }
            template <typename local_t> bool GetLocalFrom (
                local_t &rhLocal, local_value_t const &hValue
            ) const {
                return false;
            }

        //  ... handle -> local handle (maybe) implementations
            bool GetLocalFrom (local_value_t &rhLocal, local_value_t const &hValue) const {
                rhLocal = hValue;
                return true;
            }

            bool GetLocalFrom (V8<v8::Boolean>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToBoolean (context ()));
            }
            bool GetLocalFrom (V8<v8::Function>::local &rhLocal, local_value_t hValue) const {
                if (hValue->IsFunction ()) {
                    rhLocal = local_function_t::Cast (hValue);
                    return true;
                }
                return false;
            }
            bool GetLocalFrom (V8<v8::Integer>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToInteger (context ()));
            }
            bool GetLocalFrom (V8<v8::Int32>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToInt32 (context ()));
            }
            bool GetLocalFrom (V8<v8::Number>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToNumber (context ()));
            }
            bool GetLocalFrom (V8<v8::Object>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToObject (context ()));
            }
            bool GetLocalFrom (V8<v8::String>::local &rhLocal, local_value_t hValue, bool bDetailed) const {
                return GetLocalFor (
                    rhLocal,
                    bDetailed ? hValue->ToDetailString (context ()) : hValue->ToString (context ())
                );
            }
            bool GetLocalFrom (V8<v8::Uint32>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToUint32 (context ()));
            }

        //  Access Helpers
        public:
            template <typename unwrapped_t, typename handle_t> bool GetUnwrapped (
                unwrapped_t &rUnwrapped, handle_t hValue
            ) const {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue) && GetUnwrapped (
                    rUnwrapped, hLocalValue
                );
            }

            bool GetUnwrapped (bool    &rUnwrapped, local_value_t hValue) const;
            bool GetUnwrapped (double  &rUnwrapped, local_value_t hValue) const;
            bool GetUnwrapped (int32_t &rUnwrapped, local_value_t hValue) const;

            template <typename handle_t> bool UnwrapString (
                VString &rUnwrapped, handle_t hValue, bool bDetail = false
            ) const {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue) && UnwrapString (
                    rUnwrapped, hLocalValue, bDetail
                );
            }
            bool UnwrapString (
                VString &rUnwrapped, local_value_t hValue, bool bDetail = false
            ) const;
            bool UnwrapString (VString &rUnwrapped, maybe_string_t hString) const;
            bool UnwrapString (VString &rUnwrapped, local_string_t hString) const;

        //  Creation Helpers
        public:
            local_resolver_t NewResolver () const;
            local_string_t   NewString (char const *pString) const;
            local_integer_t  NewNumber (int iNumber) const {
                return integer_t::New (handle (), iNumber);
            }
            local_number_t  NewNumber (double iNumber) const {
                return number_t::New (handle (), iNumber);
            }

        //  Exception Helpers
        public:
            void ThrowTypeError (char const *pMessage) const;

        //  Export Access
            bool GetExport (Vxa::export_return_t &rExport, local_value_t hValue);

        //  Model Management
        public:
            bool Attach (
                ExportReference &rpModelObject, maybe_value_t hValue
            );
            bool Attach (
                ExportReference &rpModelObject, local_value_t hValue
            );
        private:
            bool Detach (Export *pModelObject);

        //  Result Return
        public:
        /*----------------------*
         *----  Maybe Call  ----*
         *----------------------*/
            template <typename handle_t> bool MaybeSetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, handle_t hCallable, vxa_pack_t const &rPack
            ) {
                local_value_t hLocalCallable;
                return GetLocalFor (hLocalCallable, hCallable)
                    && MaybeSetResultToCall (rResult, hReceiver, hLocalCallable, rPack);
            }
            bool MaybeSetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, local_value_t hCallable, vxa_pack_t const &rPack
            );
            bool MaybeSetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, local_function_t hCallable, vxa_pack_t const &rPack
            );
            bool MaybeSetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, local_object_t hCallable, vxa_pack_t const &rPack
            );
            template <typename cast_callable_t> bool MaybeSetResultToCallOf (
                vxa_result_t &rResult, local_value_t hReceiver, local_value_t hCallable, vxa_pack_t const &rPack
            ) {
                cast_callable_t hCastCallable;
                return GetLocalFrom (hCastCallable, hCallable)
                    && MaybeSetResultToCall (rResult, hReceiver, hCastCallable, rPack);
            }

        /*-----------------------*
         *----  Maybe Error  ----*
         *-----------------------*/
            bool MaybeSetResultToError (vxa_result_t &rResult, v8::TryCatch &rCatcher);

        /*-----------------------*
         *----  Maybe Value  ----*
         *-----------------------*/
            template <typename handle_t> bool MaybeSetResultToValue (
                vxa_result_t &rResult, handle_t hValue
            ) {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue)
                    && MaybeSetResultToValue (rResult, hLocalValue);
            }
            bool MaybeSetResultToValue (
                vxa_result_t &rResult, local_value_t hValue
            );

            bool MaybeSetResultToInt32 (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToDouble (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToString (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToObject (
                vxa_result_t &rResult, local_value_t hValue
            );

        /*--------------------------*
         *----  SetResultTo...  ----*
         *--------------------------*/
            template <typename handle_t> bool SetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, handle_t hCallable, vxa_pack_t const &rPack
            ) {
                return MaybeSetResultToCall (rResult, hReceiver, hCallable, rPack)
                    || SetResultToUndefined (rResult);
            }
            template <typename handle_t> bool SetResultToValue (
                vxa_result_t &rResult, handle_t hValue
            ) {
                return MaybeSetResultToValue (rResult, hValue) || SetResultToUndefined (rResult);
            }

            bool SetResultToGlobal (vxa_result_t &rResult);
            bool SetResultToUndefined (vxa_result_t &rResult);

        //  State
        private:
            isolate_handle_t const m_hIsolate;
            object_cache_global_t  m_hValueCache;
        };

    } // namespace VA::Node

} // namespace VA


#endif
