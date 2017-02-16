/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
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

#ifndef HR_VERSION_H
#define HR_VERSION_H

#include <QString>

namespace hrlib
{
    struct ReleaseType
    {
        enum e { Dev, Aplha, Beta, Nightly, Release };
    };

    class Version
    {
    public:
        explicit Version(int major, int minor, int revision, int build, QString commit, ReleaseType::e releaseType);

        int Major() const { return _major; }
        int Minor() const { return _minor; }
        int Revision() const { return _revision; }
        int Build() const { return _build; }
        QString commit() const { return _commit; }
//        ReleaseType::e ReleaseType() const { return _releaseType; }

        QString toString() const;

    private:
        int _major;
        int _minor;
        int _revision;
        int _build;
        QString _commit;
        QString _string;
        ReleaseType::e _releaseType;
    };
}

#endif // HR_VERSION_H
