/*
 * Created by Rohan Kuruvilla
 * Copyright (c) 2012 Zynga Inc.
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __JS_MANUAL_CONVERSIONS_H__
#define __JS_MANUAL_CONVERSIONS_H__

#include "jsapi.h"
#include "jsfriendapi.h"
#include "scripting/js-bindings/manual/js_bindings_core.h"
#include "scripting/js-bindings/manual/js_bindings_config.h"

#include "base/CCEventMouse.h"
#include "base/CCMap.h"
#include "base/CCValue.h"
#include "base/CCVector.h"
#include "base/ccTypes.h"
#include "2d/CCLabel.h"
#include "math/CCAffineTransform.h"
#include "platform/CCPlatformMacros.h"
#include "renderer/CCGLProgram.h"
#include "scripting/js-bindings/manual/spidermonkey_specifics.h"
#include "scripting/js-bindings/manual/js-BindingsExport.h"

#define JSB_COMPATIBLE_WITH_COCOS2D_HTML5_BASIC_TYPES

NS_CC_BEGIN
struct CC_DLL ResourceData;
namespace extension {
    struct ManifestAsset;
}
NS_CC_END

// just a simple utility to avoid mem leaking when using JSString
class JSStringWrapper
{
public:
    JSStringWrapper();
    JSStringWrapper(JSString* str, JSContext* cx = NULL);
    JSStringWrapper(JS::HandleValue val, JSContext* cx = NULL);
    ~JSStringWrapper();

    void set(JS::HandleValue val, JSContext* cx);
    void set(JSString* str, JSContext* cx);
    const char* get();

private:
    const char* _buffer;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(JSStringWrapper);
};

// wraps a function and "this" object
class JSFunctionWrapper
{
public:
    JSFunctionWrapper(JSContext* cx, JS::HandleObject jsthis, JS::HandleValue fval);
    JSFunctionWrapper(JSContext* cx, JS::HandleObject jsthis, JS::HandleValue fval, JS::HandleValue owner);
    ~JSFunctionWrapper();

    void setOwner(JSContext* cx, JS::HandleValue owner);
    bool invoke(JS::HandleValueArray args, JS::MutableHandleValue rval);
private:
    JSContext *_cx;
    JS::Heap<JSObject*> _jsthis;
    JS::Heap<JS::Value> _fval;
    JS::Heap<JS::Value> _owner;
    void* _cppOwner;

    CC_DISALLOW_COPY_AND_ASSIGN(JSFunctionWrapper);
};

bool jsval_to_opaque( JSContext *cx, JS::HandleValue vp, void **out );
bool jsval_to_int( JSContext *cx, JS::HandleValue vp, int *out);
bool jsval_to_uint( JSContext *cx, JS::HandleValue vp, unsigned int *out);
bool jsval_to_c_class( JSContext *cx, JS::HandleValue vp, void **out_native, struct jsb_c_proxy_s **out_proxy);
/** converts a js value (JS string) into a char */
bool jsval_to_charptr( JSContext *cx, JS::HandleValue vp, const char **out);

JS::HandleValue opaque_to_jsval( JSContext *cx, void* opaque);
JS::HandleValue c_class_to_jsval( JSContext *cx, void* handle, JS::HandleObject object, JSClass *klass, const char* class_name);

/* Converts a char ptr into a js value (using JS string) */
JS::HandleValue charptr_to_jsval( JSContext *cx, const char *str);
bool JSB_jsval_typedarray_to_dataptr( JSContext *cx, JS::HandleValue vp, GLsizei *count, void **data, js::Scalar::Type t);
bool JSB_get_arraybufferview_dataptr( JSContext *cx, JS::HandleValue vp, GLsizei *count, GLvoid **data );

// some utility functions
// to native
bool jsval_to_ushort( JSContext *cx, JS::HandleValue vp, unsigned short *ret );
bool jsval_to_int32( JSContext *cx, JS::HandleValue vp, int32_t *ret );
bool jsval_to_uint32( JSContext *cx, JS::HandleValue vp, uint32_t *ret );
bool jsval_to_uint16( JSContext *cx, JS::HandleValue vp, uint16_t *ret );
bool jsval_to_long( JSContext *cx, JS::HandleValue vp, long *out);
bool jsval_to_ulong( JSContext *cx, JS::HandleValue vp, unsigned long *out);
bool jsval_to_long_long(JSContext *cx, JS::HandleValue v, long long* ret);
CC_JS_DLL bool jsval_to_std_string(JSContext *cx, JS::HandleValue v, std::string* ret);
bool jsval_to_ccpoint(JSContext *cx, JS::HandleValue v, cocos2d::Point* ret);
bool jsval_to_ccrect(JSContext *cx, JS::HandleValue v, cocos2d::Rect* ret);
bool jsval_to_ccsize(JSContext *cx, JS::HandleValue v, cocos2d::Size* ret);
bool jsval_to_cccolor4b(JSContext *cx, JS::HandleValue v, cocos2d::Color4B* ret);
bool jsval_to_cccolor4f(JSContext *cx, JS::HandleValue v, cocos2d::Color4F* ret);
bool jsval_to_cccolor3b(JSContext *cx, JS::HandleValue v, cocos2d::Color3B* ret);
bool jsval_cccolor_to_opacity(JSContext *cx, JS::HandleValue v, int32_t* ret);
bool jsval_to_ccarray_of_CCPoint(JSContext* cx, JS::HandleValue v, cocos2d::Point **points, int *numPoints);
bool jsval_to_ccacceleration(JSContext* cx, JS::HandleValue v, cocos2d::Acceleration* ret);
bool jsval_to_quaternion(JSContext *cx, JS::HandleValue vp, cocos2d::Quaternion* ret);
bool jsval_to_TTFConfig(JSContext *cx, JS::HandleValue vp, cocos2d::TTFConfig* ret);
bool jsval_to_resourcedata(JSContext *cx, JS::HandleValue v, cocos2d::ResourceData* ret);

// forward declaration
CC_JS_DLL js_proxy_t* jsb_get_js_proxy(JS::HandleObject jsObj);

template <class T>
bool jsvals_variadic_to_ccvector( JSContext *cx, /*JS::Value *vp, int argc,*/const JS::CallArgs& args, cocos2d::Vector<T>* ret)
{
    bool ok = true;

    for (unsigned i = 0; i < args.length(); i++)
    {
        js_proxy_t* p;
        JS::RootedObject obj(cx, args.get(i).toObjectOrNull());

        p = jsb_get_js_proxy(obj);
        CCASSERT(p, "Native object not found!");
        if (p) {
            ret->pushBack((T)p->ptr);
        }
    }

    JSB_PRECONDITION3(ok, cx, false, "Error processing arguments");
    return ok;
}

bool jsvals_variadic_to_ccvaluevector( JSContext *cx, JS::Value *vp, int argc, cocos2d::ValueVector* ret);

bool jsval_to_ccaffinetransform(JSContext* cx, JS::HandleValue v, cocos2d::AffineTransform* ret);
bool jsval_to_FontDefinition( JSContext *cx, JS::HandleValue vp, cocos2d::FontDefinition* ret );

template <class T>
bool jsval_to_ccvector(JSContext* cx, JS::HandleValue v, cocos2d::Vector<T>* ret)
{
    JS::RootedObject jsobj(cx);
    if (v.isObject())
    {
        jsobj = v.toObjectOrNull();
    }
    else
    {
        return false;
    }
    bool isArray;
    JSB_PRECONDITION3(JS_IsArrayObject(cx, jsobj, &isArray) && isArray, cx, false, "Object must be an array");

    uint32_t len = 0;
    JS_GetArrayLength(cx, jsobj, &len);

    for (uint32_t i=0; i < len; i++)
    {
        JS::RootedValue value(cx);
        if (JS_GetElement(cx, jsobj, i, &value))
        {
            CCASSERT(value.isObject(), "the element in Vector isn't a native object.");

            js_proxy_t *proxy;
            JS::RootedObject tmp(cx, value.toObjectOrNull());
            proxy = jsb_get_js_proxy(tmp);
            T cobj = (T)(proxy ? proxy->ptr : nullptr);
            if (cobj)
            {
                ret->pushBack(cobj);
            }
        }
    }

    return true;
}

bool jsval_to_ccvalue(JSContext* cx, JS::HandleValue v, cocos2d::Value* ret);
bool jsval_to_ccvaluemap(JSContext* cx, JS::HandleValue v, cocos2d::ValueMap* ret);
bool jsval_to_ccvaluemapintkey(JSContext* cx, JS::HandleValue v, cocos2d::ValueMapIntKey* ret);
bool jsval_to_ccvaluevector(JSContext* cx, JS::HandleValue v, cocos2d::ValueVector* ret);
bool jsval_to_ssize( JSContext *cx, JS::HandleValue vp, ssize_t* ret);
CC_JS_DLL bool jsval_to_std_vector_string( JSContext *cx, JS::HandleValue vp, std::vector<std::string>* ret);
bool jsval_to_std_vector_int( JSContext *cx, JS::HandleValue vp, std::vector<int>* ret);
bool jsval_to_std_vector_float( JSContext *cx, JS::HandleValue vp, std::vector<float>* ret);
bool jsval_to_matrix(JSContext *cx, JS::HandleValue vp, cocos2d::Mat4* ret);
bool jsval_to_vector2(JSContext *cx, JS::HandleValue vp, cocos2d::Vec2* ret);
bool jsval_to_vector3(JSContext *cx, JS::HandleValue vp, cocos2d::Vec3* ret);
bool jsval_to_vector4(JSContext *cx, JS::HandleValue vp, cocos2d::Vec4* ret);
bool jsval_to_blendfunc(JSContext *cx, JS::HandleValue vp, cocos2d::BlendFunc* ret);
bool jsval_to_vector_vec2(JSContext* cx, JS::HandleValue v, std::vector<cocos2d::Vec2>* ret);
bool jsval_to_cctex2f(JSContext* cx, JS::HandleValue vp, cocos2d::Tex2F* ret);
bool jsval_to_v3fc4bt2f(JSContext* cx, JS::HandleValue v, cocos2d::V3F_C4B_T2F* ret);
bool jsval_to_v3fc4bt2f_quad(JSContext* cx, JS::HandleValue v, cocos2d::V3F_C4B_T2F_Quad* ret);
bool jsval_to_vector_v3fc4bt2f(JSContext* cx, JS::HandleValue v, std::vector<cocos2d::V3F_C4B_T2F>* ret);
bool jsval_to_std_map_string_string(JSContext* cx, JS::HandleValue v, std::map<std::string, std::string>* ret);

template <class T>
bool jsval_to_ccmap_string_key(JSContext *cx, JS::HandleValue v, cocos2d::Map<std::string, T>* ret)
{
    if (v.isNullOrUndefined())
    {
        return true;
    }
    
    JS::RootedObject tmp(cx, v.toObjectOrNull());
    if (!tmp)
    {
        CCLOG("%s", "jsval_to_ccmap_string_key: the js value is not an object.");
        return false;
    }
    
    JS::Rooted<JS::IdVector> ids(cx, cx);
    if (!JS_Enumerate(cx, tmp, &ids))
    {
        CCLOG("%s", "jsval_to_ccmap_string_key: Failed to enumerate the js object.");
        return false;
    }
    
    JS::RootedId idp(cx);
    JS::RootedValue key(cx);
    JS::RootedValue value(cx);
    JS::RootedObject jsobj(cx);
    for (int i = 0; i < ids.length(); ++i)
    {
        idp = ids[i];
        if (!JS_IdToValue(cx, idp, &key))
        {
            return false; // error
        }
        if (!key.isString())
        {
            continue; // ignore integer properties
        }
        JSStringWrapper keyWrapper(key.toString(), cx);

        JS_GetPropertyById(cx, tmp, idp, &value);
        if (value.isObject())
        {
            js_proxy_t *proxy = nullptr;
            jsobj = value.toObjectOrNull();
            proxy = jsb_get_js_proxy(jsobj);
            CCASSERT(proxy, "Native object should be added!");
            T cobj = (T)(proxy ? proxy->ptr : nullptr);
            ret->insert(keyWrapper.get(), cobj);
        }
        else
        {
            CCASSERT(false, "not supported type");
        }
    }

    return true;
}

// from native
JS::HandleValue int32_to_jsval( JSContext *cx, int32_t l);
JS::HandleValue uint32_to_jsval( JSContext *cx, uint32_t number );
JS::HandleValue ushort_to_jsval( JSContext *cx, unsigned short number );
JS::HandleValue long_to_jsval( JSContext *cx, long number );
JS::HandleValue ulong_to_jsval(JSContext* cx, unsigned long v);
JS::HandleValue long_long_to_jsval(JSContext* cx, long long v);
CC_JS_DLL JS::HandleValue std_string_to_jsval(JSContext* cx, const std::string& v);
JS::HandleValue c_string_to_jsval(JSContext* cx, const char* v, size_t length = -1);
JS::HandleValue ccpoint_to_jsval(JSContext* cx, const cocos2d::Point& v);
JS::HandleValue ccrect_to_jsval(JSContext* cx, const cocos2d::Rect& v);
JS::HandleValue ccsize_to_jsval(JSContext* cx, const cocos2d::Size& v);
JS::HandleValue cccolor4b_to_jsval(JSContext* cx, const cocos2d::Color4B& v);
JS::HandleValue cccolor4f_to_jsval(JSContext* cx, const cocos2d::Color4F& v);
JS::HandleValue cccolor3b_to_jsval(JSContext* cx, const cocos2d::Color3B& v);
JS::HandleValue ccacceleration_to_jsval(JSContext* cx, const cocos2d::Acceleration& v);
JS::HandleValue ccaffinetransform_to_jsval(JSContext* cx, const cocos2d::AffineTransform& t);
JS::HandleValue FontDefinition_to_jsval(JSContext* cx, const cocos2d::FontDefinition& t);
JS::HandleValue quaternion_to_jsval(JSContext* cx, const cocos2d::Quaternion& q);
JS::HandleValue uniform_to_jsval(JSContext* cx, const cocos2d::Uniform* uniform);
JS::HandleValue resourcedata_to_jsval(JSContext* cx, const cocos2d::ResourceData& v);
JS::HandleValue asset_to_jsval(JSContext* cx, const cocos2d::extension::ManifestAsset& v);

// forward declaration
template <class T>
js_type_class_t *js_get_type_from_native(T* native_obj);

// Ref version of ccvector_to_jsval
template <class T>
JS::HandleValue ccvector_to_jsval(JSContext* cx, const cocos2d::Vector<T>& v)
{
    JS::RootedObject jsretArr(cx, JS_NewArrayObject(cx, 0));

    int i = 0;
    for (const auto& obj : v)
    {
        JS::RootedValue arrElement(cx);

        //First, check whether object is associated with js object.
        js_type_class_t *typeClass = js_get_type_from_native(obj);
        JS::RootedObject jsobject(cx, jsb_ref_get_or_create_jsobject(cx, obj, typeClass, typeid(*obj).name()));
        if (jsobject.get()) {
            arrElement = JS::ObjectOrNullValue(jsobject.get());
        }

        if (!JS_SetElement(cx, jsretArr, i, arrElement)) {
            break;
        }
        ++i;
    }
    JS::RootedValue ret(cx, JS::ObjectOrNullValue(jsretArr));
    return ret;
}

template <class T>
JS::HandleValue ccmap_string_key_to_jsval(JSContext* cx, const cocos2d::Map<std::string, T>& v)
{
    JS::RootedObject jsRet(cx, JS_NewPlainObject(cx));

    for (auto iter = v.begin(); iter != v.end(); ++iter)
    {
        JS::RootedValue element(cx);

        std::string key = iter->first;
        T obj = iter->second;

        //First, check whether object is associated with js object.
        js_type_class_t *typeClass = js_get_type_from_native(obj);
        JS::RootedObject jsobject(cx, jsb_ref_get_or_create_jsobject(cx, obj, typeClass, typeid(*obj).name()));

        if (jsobject.get()) {
            element = JS::ObjectOrNullValue(jsobject);
        }

        if (!key.empty())
        {
            JS_SetProperty(cx, jsRet, key.c_str(), element);
        }
    }
    JS::RootedValue retVal(cx, JS::ObjectOrNullValue(jsRet));
    return retVal;
}

JS::HandleValue ccvalue_to_jsval(JSContext* cx, const cocos2d::Value& v);
JS::HandleValue ccvaluemap_to_jsval(JSContext* cx, const cocos2d::ValueMap& v);
JS::HandleValue ccvaluemapintkey_to_jsval(JSContext* cx, const cocos2d::ValueMapIntKey& v);
JS::HandleValue ccvaluevector_to_jsval(JSContext* cx, const cocos2d::ValueVector& v);
JS::HandleValue ssize_to_jsval(JSContext *cx, ssize_t v);
JS::HandleValue std_vector_string_to_jsval( JSContext *cx, const std::vector<std::string>& v);
JS::HandleValue std_vector_int_to_jsval( JSContext *cx, const std::vector<int>& v);
JS::HandleValue std_vector_float_to_jsval( JSContext *cx, const std::vector<float>& v);
JS::HandleValue matrix_to_jsval(JSContext *cx, const cocos2d::Mat4& v);
JS::HandleValue vector2_to_jsval(JSContext *cx, const cocos2d::Vec2& v);
JS::HandleValue vector3_to_jsval(JSContext *cx, const cocos2d::Vec3& v);
JS::HandleValue vector4_to_jsval(JSContext *cx, const cocos2d::Vec4& v);
JS::HandleValue blendfunc_to_jsval(JSContext *cx, const cocos2d::BlendFunc& v);
JS::HandleValue vector_vec2_to_jsval(JSContext *cx, const std::vector<cocos2d::Vec2>& v);
JS::HandleValue std_map_string_string_to_jsval(JSContext* cx, const std::map<std::string, std::string>& v);

#endif /* __JS_MANUAL_CONVERSIONS_H__ */
