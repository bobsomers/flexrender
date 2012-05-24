#pragma once

#include <cstdlib>
#include <string>

#include "glm/glm.hpp"
#include "luajit-2.0/lua.hpp"

#include "utils/tout.hpp"
#include "utils/uncopyable.hpp"

// The linker guarantees the memory address of this constant will be unique,
// so we can use that memory address as a unique key into the Lua registry.
// Super hacktastic Batman!
static const char LUA_ENGINE_REGISTRY_KEY = 'k';

// Horrendous, evil macro for automatically defining a resolver and handler
// function in subclasses which are based on Script. Sometimes you gotta
// pay the price for simple interfaces...
#define FR_SCRIPT_FUNCTION(class_name, func_name) \
    int class_name::func_name##_RESOLVER(lua_State *L) { \
        class_name *obj = Script::RetrieveObjectPointer<class_name>(L); \
        return obj->func_name##_HANDLER(); \
    } \
    int class_name::func_name##_HANDLER()

// Another horrendous, evil macro for defining the functions that will be
// automatically created with the above macro. Same caveat as above.
#define FR_SCRIPT_DECLARE(func_name) \
    static int func_name##_RESOLVER(lua_State *L); \
    int func_name##_HANDLER()

// Another horrendous, evil macro for registering the resolver and handler
// functions created with the previous macro with the underlying Lua
// interpreter. Same caveat as above.
#define FR_SCRIPT_REGISTER(lua_func_name, class_name, func_name) \
    lua_register(_state, lua_func_name, class_name::func_name##_RESOLVER)

// Another horrendous, evil macro for initializing the object pointers
// correctly. Same caveat as above.
#define FR_SCRIPT_INIT(class_name, which_libs) \
    Script::Init(which_libs); \
    StoreObjectPointer<class_name>(this)

namespace fr {

class Script : private Uncopyable {
public:
    explicit Script();
    virtual ~Script();

protected:
    /// Defines the set of libraries that the script interpreter has access to.
    enum class ScriptLibs {
        NO_LIBS,
        STANDARD_LIBS
    };

    /// The Lua interpreter state.
    lua_State *_state;

    /**
     * Initializes a new script interpreter, with access to the passed set of
     * libraries.
     *
     * @param   libs    The set of libraries available to the interpreter.
     */
    virtual void Init(ScriptLibs libs = ScriptLibs::NO_LIBS);

    /**
     * Prints out the entire contents of the Lua stack. Useful for debugging.
     */
    void DumpStack();

    /**
     * Kills script execution with the given error message.
     */
    void ScriptError(const std::string& message);

    /**
     * This will call a Lua function with num_args arguments pushed onto the
     * stack and will return with num_returns return values on the stack. The
     * original function and its arguments will be popped automatically by
     * Lua.
     */
    void CallFunc(int num_args, int num_returns);

    /**
     * Verifies that the first argument to this function call was a table, and
     * pushes a copy of that table onto the top of the stack (so all indexing
     * can be done with negative indices).
     */
    void BeginTableCall();

    /**
     * Pops the copy of the table we pushed off the stack.
     */
    void EndTableCall();

    /**
     * Pushes a string representation of the given resource ID onto the stack
     * and returns the number of arguments pushed (1).
     */
    int ReturnResourceID(uint32_t id);

    /**
     * Takes a string representation of a resource ID and decodes it back to
     * a 32-bit unsigned integer.
     */
    uint32_t DecodeResourceID(std::string value);

    /**
     * Verifies that the top of the stack has the passed type. You may pass an
     * optional cosmetic name of the thing you're type checking to make the
     * error message better.
     */
    void TypeCheck(int type, const std::string& name = "");

    /**
     * Assuming the table is on the top of the stack, push the field named
     * name onto the stack if it passes a type check of the given luatype.
     * Returns true if the field was found, false if it wasnt.
     */
    bool PushField(const char* name, int field_type);

    /**
     * Removed the pushed field from the stack.
     */
    void PopField();

    /**
     * Runs the given function over all numerically indexed elements in the
     * table on the top of the stack. Passes into that function the index of
     * the current element.
     */
    void ForEachIndex(std::function<void (size_t)> func);

    /**
     * Pushes the numerical index of a table (which should be on the top of
     * the stack) onto the stack. Does a type check to make sure that the
     * pushed value is of passed type index_type.
     */
    void PushIndex(size_t index, int index_type);

    /**
     * Pops a pushed index value off the stack.
     */
    void PopIndex();

    /**
     * Iterates over the key/value pairs in a table that is on the top of the
     * stack. The key is passed in to the lambda, but you DO NOT need to do
     * anything to get the value. The value will be sitting on the top of the
     * stack when the lambda is called.
     */
    void ForEachKeyVal(std::function<void (const std::string &key)> func);

    /**
     * Returns the top of the stack as a string. It is not type checked.
     */
    std::string FetchString();

    /**
     * Returns the top of the stack as a boolean. It is not type checked.
     */
    bool FetchBool();

    /**
     * Returns the top of the stack as a float. It is not type checked.
     */
    float FetchFloat();

    /**
     * Pushes the given float onto the top of the stack.
     */
    void PushFloat(float f);

    /**
     * Fetches a 2-dimensional vector as a table of 2 floats. It typechecks the
     * elements of the vector. Assumes the table is on the top of the stack.
     */
    glm::vec2 FetchFloat2();

    /**
     * Pushes the given 2-dimensional vector as a table of 2 floats onto the
     * top of the stack.
     */
    void PushFloat2(glm::vec2 v);

    /**
     * Fetches a 3-dimensional vector as a table of 3 floats. It typechecks the
     * elements of the vector. Assumes the table is on the top of the stack.
     */
    glm::vec3 FetchFloat3();

    /**
     * Pushes the given 3-dimensional vector as a table of 3 floats onto the
     * top of the stack.
     */
    void PushFloat3(glm::vec3 v);

    /**
     * Fetches a 4-dimensional vector as a table of 4 floats. It typechecks the
     * elements of the vector. Assumes the table is on the top of the stack.
     */
    glm::vec4 FetchFloat4();

    /**
     * Pushes the given 4-dimensional vector as a table of 4 floats onto the
     * top of the stack.
     */
    void PushFloat4(glm::vec4 v);

    /**
     * Helper function for squirreling away a pointer to the object instance
     * inside the Lua interpreter registry. This works in conjuction with the
     * above macros to provide a much simpler interface for FlexScript
     * subclasses. You shouldn't call this directly. Let the macro do its
     * magic.
     */
    template <typename T>
    void StoreObjectPointer(T *ptr) {
        if (_state == nullptr) {
            TERRLN("Attempt to store object pointer before initializing script engine.");
            exit(EXIT_FAILURE);
        }

        lua_pushlightuserdata(_state,
         reinterpret_cast<void *>(const_cast<char *>(&LUA_ENGINE_REGISTRY_KEY)));
        lua_pushlightuserdata(_state, reinterpret_cast<void *>(ptr));
        lua_settable(_state, LUA_REGISTRYINDEX);
    }

    /** 
     * Helper function for unsquirreling a pointer to the object instance from
     * the Lua interpreter registry. This works in conjunction with the above
     * macros to provide a much simpler interface for ScriptEngine subclasses.
     * You shouldn't call this directly. Let the macro do its magic.
     */
    template <typename T>
    static T *RetrieveObjectPointer(lua_State *L) {
        if (L == nullptr) {
            TERRLN("Attempt to retrieve object pointer before initializing Lua engine.");
            exit(EXIT_FAILURE);
        }

        lua_pushlightuserdata(L,
         reinterpret_cast<void *>(const_cast<char *>(&LUA_ENGINE_REGISTRY_KEY)));
        lua_gettable(L, LUA_REGISTRYINDEX);
        return reinterpret_cast<T *>(lua_touserdata(L, -1));
    }
};

} // namespace fr
