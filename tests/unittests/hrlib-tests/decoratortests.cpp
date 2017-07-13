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

#include "decoratortests.hpp"

#include "submodules/qtestrunner/qtestrunner.hpp"
#include "hrlib/patterns/decorator.hpp"

using namespace hrlib::patterns;

namespace // Anonymous namespace containing test classes
{
    enum STATE {
        NONE,
        NEW_TARGET, DEL_TARGET,
        NEW_DECORATOR, DEL_DECORATOR,
        NEW_INIT_DEC, DEL_INIT_DEC
    };
    STATE _state = NONE;

    class Interface
    {
    public:
        virtual ~Interface() {}
        virtual double value() = 0;
    };

    class Target : public Interface
    {
    public:
        virtual double value() override { return 2; }

        explicit Target() { _state = NEW_TARGET; }
        virtual ~Target() { _state = DEL_TARGET; }
    };

    class SimpleDecorator : public Decorator<Interface>
    {
    public:
        virtual double value() override { return _target->value() * 2; }

        explicit SimpleDecorator(Interface *target) : Decorator(target) { _state = NEW_DECORATOR; }
        explicit SimpleDecorator(std::shared_ptr<Interface> target) : Decorator(target) { _state = NEW_DECORATOR; }
        explicit SimpleDecorator(std::unique_ptr<Interface>&& target) : Decorator(std::move(target)) { _state = NEW_DECORATOR; }
        virtual ~SimpleDecorator() { _state = DEL_DECORATOR; }
    };

    class CtorDecorator : public Decorator<Interface>
    {
    public:
        int _value;

        virtual double value() override { return _target->value() * _value; }

        explicit CtorDecorator(Interface *target, int value) : Decorator(target) { _value = value; }
    };

    class InitDecorator : public Decorator<Interface>
    {
    public:
        int _first;
        float _second;

        virtual double value() override { return _target->value() * _first + _second; }

        explicit InitDecorator(Interface *target) : Decorator(target) { _state = NEW_INIT_DEC; }
        explicit InitDecorator(std::unique_ptr<Interface>&& target) : Decorator(std::move(target)) { _state = NEW_INIT_DEC; }
        void init(int first, float second) { _first = first; _second = second; }
        virtual ~InitDecorator() { _state = DEL_INIT_DEC; }
    };
} // End anonymous namespace containing test classes

void DecoratorTests::testNoArgs()
{
    //
    // Test a decorator without arguments
    //

    auto target = std::unique_ptr<Target>(new Target());

    auto decorated = decorate<SimpleDecorator>(target.get());

    QCOMPARE(target->value(), 2.0);
    QCOMPARE(decorated->value(), target->value() * 2); // 2 * 2 = 4
}

void DecoratorTests::testContructorArgs()
{
    //
    // Test a decorator with constructor arguments
    //

    auto target = std::unique_ptr<Target>(new Target());

    // construct with: CtorDecorator(target.get(), 3);
    auto decorated = decorate<CtorDecorator>(target.get(), 3);

    QCOMPARE(target->value(), 2.0);
    QCOMPARE(decorated->value(), target->value() * 3); // 2 * 3 = 6
}

void DecoratorTests::testInitArgs()
{
    //
    // Test a decorator with an init method with arguments
    //

    auto target = std::unique_ptr<Target>(new Target());

    // after construction init with: init(10, -1.5f);
    auto decorated = decorate<InitDecorator>(target.get(), 10, -1.5f);

    QCOMPARE(target->value(), 2.0);
    QCOMPARE(decorated->value(), target->value() * 10 - 1.5); // 2 * 10 - 1.5 = 18.5
}

void DecoratorTests::testRawPointerOwnership()
{
    //
    // Test a non-owning decorator (passing a raw pointer)
    //

    _state = NONE;

    { // start target scope

        Target target;
        QCOMPARE(_state, NEW_TARGET);

        { // start decorated scope

            auto decorated = decorate<SimpleDecorator>(&target);
            QCOMPARE(_state, NEW_DECORATOR);

        } // end decorated scope

        QCOMPARE(_state, DEL_DECORATOR);

    } // end target scope

    QCOMPARE(_state, DEL_TARGET);


    _state = NONE;

    { // start decorated scope

        std::unique_ptr<Interface> decorated;

        { // start target scope

            Target target;
            QCOMPARE(_state, NEW_TARGET);

            decorated = decorate<SimpleDecorator>(&target);
            QCOMPARE(_state, NEW_DECORATOR);

        } // end target scope

        QCOMPARE(_state, DEL_TARGET);

    } // end decorated scope

    QCOMPARE(_state, DEL_DECORATOR);
}

void DecoratorTests::testUniquePointerOwnership()
{
    //
    // Test an owning decorator (moving a unique pointer)
    //

    _state = NONE;

    { // start target scope

        auto target = std::unique_ptr<Interface>(new Target());
        QCOMPARE(_state, NEW_TARGET);

        { // start decorated scope

            auto decorated = decorate<SimpleDecorator>(std::move(target));
            QCOMPARE(_state, NEW_DECORATOR);

        } // end decorated scope

        // Target destructed together with decorator: ownership transferred to decorator
        QCOMPARE(_state, DEL_TARGET);

    } // end target scope

    QCOMPARE(_state, DEL_TARGET);



    _state = NONE;

    { // start decorated scope

        std::unique_ptr<Interface> decorated;

        { // start target scope

            auto target = std::unique_ptr<Interface>(new Target());
            QCOMPARE(_state, NEW_TARGET);

            decorated = decorate<SimpleDecorator>(std::move(target));
            QCOMPARE(_state, NEW_DECORATOR);

        } // end target scope

        // target not destructed yet: ownership transferred to decorator
        QCOMPARE(_state, NEW_DECORATOR);

    } // end decorated scope

    QCOMPARE(_state, DEL_TARGET);
}

void DecoratorTests::testSharedPointerOwnership()
{
    //
    // Test a decorator with shared ownership (passing a shared pointer)
    //

    _state = NONE;

    { // start target scope

        auto target = std::shared_ptr<Interface>(new Target());
        QCOMPARE(_state, NEW_TARGET);

        { // start decorated scope

            auto decorated = decorate<SimpleDecorator>(target);
            QCOMPARE(_state, NEW_DECORATOR);

        } // end decorated scope

        // target not destructed yet: shared ownership
        QCOMPARE(_state, DEL_DECORATOR);

    } // end target scope

    QCOMPARE(_state, DEL_TARGET);


    _state = NONE;

    { // start other scope

        std::shared_ptr<Interface> other;

        { // start decorated scope

            std::unique_ptr<Interface> decorated;

            { // start target scope

                auto target = std::shared_ptr<Interface>(new Target());
                QCOMPARE(_state, NEW_TARGET);

                decorated = decorate<SimpleDecorator>(target);
                QCOMPARE(_state, NEW_DECORATOR);

                // share target ownership with other
                other = target;

            } // end target scope

            // target not destructed yet: shared ownership with other
            QCOMPARE(_state, NEW_DECORATOR);

        } // end decorated scope

        QCOMPARE(_state, DEL_DECORATOR);

    } // end other scope

    QCOMPARE(_state, DEL_TARGET);
}

void DecoratorTests::testMultipleDecoration()
{
    //
    // Test decorating a decorator (and ownership)
    //

    _state = NONE;

    { // start target scope

        auto target = std::unique_ptr<Target>(new Target());
        QCOMPARE(target->value(), 2.0);
        QCOMPARE(_state, NEW_TARGET);

        { // start firstDeco scope

            auto firstDeco = decorate<SimpleDecorator>(target.get());
            auto expected1 = target->value() * 2;
            QCOMPARE(firstDeco->value(), expected1);
            QCOMPARE(_state, NEW_DECORATOR);

            { // start secondDeco scope

                auto secondDeco = decorate<InitDecorator>(std::move(firstDeco), -2, 3);
                QCOMPARE(secondDeco->value(), expected1 * -2 + 3);
                QCOMPARE(_state, NEW_INIT_DEC);

            } // start secondDeco scope

            QCOMPARE(_state, DEL_DECORATOR);

        } // end firstDeco scope

        QCOMPARE(_state, DEL_DECORATOR);

    } // end target scope

    QCOMPARE(_state, DEL_TARGET);
}

QTR_ADD_TEST(DecoratorTests)
