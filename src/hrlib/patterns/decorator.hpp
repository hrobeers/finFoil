/****************************************************************************

 Copyright (c) 2014, Hans Robeers
 All rights reserved.

 BSD 2-Clause License

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#ifndef HRLIB_DECORATOR_H
#define HRLIB_DECORATOR_H

#include <memory>
#include <type_traits>

namespace hrlib { namespace patterns
{
    // SFINAE class to check for an init method
    template <typename T>
    class has_init
    {
        typedef char yes[1];
        typedef char no [2];
        template<typename C> static yes& test(decltype(&C::init)); // selected if C has an init method
        template<typename C> static no&  test(...);      // selected otherwise

    public:
        enum { _true = sizeof(test<T>(0)) == sizeof(yes),
               _false = sizeof(test<T>(0)) == sizeof(no) };
    };

    // decoration method for decorators without init method
    template<typename Decorator, typename... T>
    typename std::enable_if<has_init<Decorator>::_false, std::unique_ptr<typename Decorator::interface>>::type
        decorate(typename Decorator::interface *target, T&&... t)
        { return std::unique_ptr<typename Decorator::interface>(new Decorator(target, std::forward<T>(t)...)); }

    // decoration method for decorators with init method
    template<typename Decorator, typename... T>
    typename std::enable_if<has_init<Decorator>::_true, std::unique_ptr<typename Decorator::interface>>::type
        decorate(typename Decorator::interface *target, T&&... t)
        {
            auto retVal = std::unique_ptr<typename Decorator::interface>(new Decorator(target));
            ((Decorator*)retVal.get())->init(std::forward<T>(t)...);
            return retVal;
        }

    // decorator base class
    template<typename Interface>
    class Decorator : public Interface
    {
    private:
        std::shared_ptr<Interface> _sPtr;
        std::unique_ptr<Interface> _uPtr;

    protected:
        Interface *_ptr;

    public:
        typedef Interface interface;

        explicit Decorator(Interface *path) : _ptr(path) {}
        explicit Decorator(std::shared_ptr<Interface> path) : _sPtr(std::move(path)), _ptr(_sPtr.get()) {}
        explicit Decorator(std::unique_ptr<Interface> path) : _uPtr(std::move(path)), _ptr(_uPtr.get()) {}
    };
} }

#endif // HRLIB_DECORATOR_H
