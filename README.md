osIROSE - Open Source International R.O.S.E Online Server
===================================================

[![Join the chat at https://gitter.im/RavenX8/osIROSE-new](https://badges.gitter.im/RavenX8/osIROSE-new.svg)](https://gitter.im/RavenX8/osIROSE-new?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Build status
| [Linux][lin-link] | [Windows][win-link] | [Coveralls][cov-link] |
| :---------------: | :-----------------: | :-------------------: |
| ![lin-badge]      | ![win-badge]        | ![cov-badge]          |

[lin-badge]: https://travis-ci.org/RavenX8/osIROSE-new.svg?branch=trunk "Travis build status"
[lin-link]:  https://travis-ci.org/RavenX8/osIROSE-new "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/20x0eufp7djvunf3/branch/trunk?svg=true "AppVeyor build status"
[win-link]:  https://ci.appveyor.com/project/RavenX8/osirose-new/branch/trunk "AppVeyor build status"
[cov-badge]: https://coveralls.io/repos/RavenX8/osIROSE-new/badge.svg?branch=trunk&service=github
[cov-link]:  https://coveralls.io/github/RavenX8/osIROSE-new?branch=trunk

[Official website](http://forum.dev-osrose.com/index.php)

C++ Installation - Unix
-----------------------

If you get the source from github, you need to setup the build environment:

    $ ./setupEnv.sh

This will install all of the tools and sources required to build the project.

"./setupEnv.sh" may require superuser privileges, as it calls apt-get install.

To build and install the servers, execute the following:

    $ make
    $ make install

"make install" may require superuser privileges.

To run the servers execute the following:

    $ LoginServer&
    $ CharServer&
    $ MapServer&

C++ Installation - Windows
-----------------------

To compile the servers run the following commands:

    ./ci/msvc_install.bat
    cd build
    cmake ..
    cmake --build .
    
This will setup the build directories and compile. The compiled server will be in the bin folder created in the directory you ran cmake in.
