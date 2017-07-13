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

#ifndef HRLIB_DECORATOR_HPP
#define HRLIB_DECORATOR_HPP

#include <memory>
#include <type_traits>

namespace hrlib { namespace patterns
{
    // SFINAE class to check for an init method
    template <typename T>
    class has_init
    {
    private:
        typedef char _yes[1];
        typedef char _no [2];
        template<typename C> static _yes& test(decltype(&C::init));  // selected if C has an init method
        template<typename C> static _no&  test(...);                 // selected otherwise

    public:
        enum { yes = sizeof(test<T>(0)) == sizeof(_yes),
               no  = sizeof(test<T>(0)) == sizeof(_no) };
    };

    // decoration method for decorators without init method
    template<typename Decorator, typename T, typename... Args>
    typename std::enable_if<has_init<Decorator>::no, std::unique_ptr<typename Decorator::interface>>::type
        decorate(T target, Args&&... t)
        {
            return std::unique_ptr<typename Decorator::interface>(new Decorator(std::move(target), std::forward<Args>(t)...));
        }

    // decoration method for decorators with init method
    template<typename Decorator, typename T, typename... Args>
    typename std::enable_if<has_init<Decorator>::yes, std::unique_ptr<typename Decorator::interface>>::type
        decorate(T target, Args&&... t)
        {
            auto retVal = std::unique_ptr<typename Decorator::interface>(new Decorator(std::move(target)));
            ((Decorator*)retVal.get())->init(std::forward<Args>(t)...);
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
        Interface *_target;

    public:
        typedef Interface interface;

        explicit Decorator(Interface *target) : _target(target) {}
        explicit Decorator(std::shared_ptr<Interface> target) : _sPtr(target), _target(_sPtr.get()) {}
        explicit Decorator(std::unique_ptr<Interface>&& target) : _uPtr(std::move(target)), _target(_uPtr.get()) {}
    };
} }

#endif // HRLIB_DECORATOR_HPP
