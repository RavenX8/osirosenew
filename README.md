osIROSE - Open Source International R.O.S.E Online Server
===================================================

##### Build status
[![lin-badge]][lin-link] [![win-badge]][win-link]
[![cov-badge]][cov-link] [![ccov-badge]][ccov-link]
[![coverity-scan]][coverity-scan-link]

[lin-badge]: https://travis-ci.org/dev-osrose/osIROSE-new.svg?branch=trunk "Linux build status"
[lin-link]:  https://travis-ci.org/dev-osrose/osIROSE-new "Linux build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/20x0eufp7djvunf3/branch/trunk?svg=true "Windows build status"
[win-link]:  https://ci.appveyor.com/project/RavenX8/osirose-new/branch/trunk "Windows build status"
[cov-badge]: https://coveralls.io/repos/github/dev-osrose/osIROSE-new/badge.svg?branch=trunk&service=github
[cov-link]:  https://coveralls.io/github/dev-osrose/osIROSE-new?branch=trunk
[ccov-badge]: https://codecov.io/gh/dev-osrose/osIROSE-new/branch/trunk/graph/badge.svg
[ccov-link]: https://codecov.io/gh/dev-osrose/osIROSE-new/branch/trunk
[coverity-scan]: https://scan.coverity.com/projects/7232/badge.svg
[coverity-scan-link]: https://scan.coverity.com/projects/dev-osrose-osirose-new


##### Development and Community:
[![GitHub Repository](https://img.shields.io/badge/github-dev--osrose/osIROSE--new-green.svg)](https://github.com/dev-osrose/osIROSE-new)
[![Waffle Board](https://img.shields.io/badge/waffle-dev--osrose/osIROSE--new-6699dd.svg)](https://waffle.io/dev-osrose/osIROSE-new)
[![Community Forum](https://img.shields.io/badge/forum-http%3A%2F%2Fforum.dev--osrose.com-green.svg)](http://forum.dev-osrose.com/index.php)
[![Join the chat in Discord](https://img.shields.io/discord/454345032846016515.svg)](https://discord.gg/u7UE5fW)

##### Issues and Status
[![GitHub issues](https://img.shields.io/github/issues/dev-osrose/osIROSE-new.svg)](https://github.com/dev-osrose/osIROSE-new/issues)
[![Confirmed issues](https://badge.waffle.io/dev-osrose/osIROSE-new.svg?label=status:confirmed&title=Confirmed%20Issues)](http://waffle.io/dev-osrose/osIROSE-new) [![In-Progress](https://badge.waffle.io/dev-osrose/osIROSE-new.svg?label=status:in-progress&title=In%20Progress)](http://waffle.io/dev-osrose/osIROSE-new)

##### Project Info:
![Language](https://img.shields.io/badge/language-C++-yellow.svg)
[![GitHub license](https://img.shields.io/badge/license-Apache%202-blue.svg)](https://raw.githubusercontent.com/dev-osrose/osIROSE-new/master/LICENSE.txt)

C++ Installation - Unix
-----------------------

If you get the source from github, you need to setup the build environment:

    $ ./unix_env_setup.sh

This will install all of the tools and sources required to build the project.

"./unix_env_setup.sh" will require superuser privileges, as it calls apt-get install.

To build the servers, execute the following:

    $ make

To install the servers to the system (not recommended), execute the following commands:

    $ make install

"make install" may require superuser privileges.

If you installed the servers, to run the servers execute the following:

    $ LoginServer&
    $ CharServer&
    $ MapServer&

If you opted to keep them in place, the server binaries are located in the bin folder inside the build directory. So from where you ran the command make, you will want to run the following:

    $ ./bin/LoginServer&
    $ ./bin/CharServer&
    $ ./bin/MapServer&

After running for the first time, the servers will create a config file in the current directory.

C++ Installation - Windows
-----------------------

##### Programs required:
[![vis-studio]][vis-studio-link]
[![cmake]][cmake-link]
[![mysql]][mysql-link]
[![python]][python-link]

[vis-studio]: https://img.shields.io/badge/Visual%20Studio-Download-blue.svg "Download Visual Stuido"
[vis-studio-link]: https://visualstudio.microsoft.com/downloads/ "Download Visual Stuido"
[cmake]: https://img.shields.io/badge/CMake-Download-blue.svg "Download CMake"
[cmake-link]: https://cmake.org/download/ "Download CMake"
[mysql]: https://img.shields.io/badge/MySQL%20Connector%20C%2064--bit-Download-blue.svg "Download MySQL Connector:C 64-bit"
[mysql-link]: http://dev.mysql.com/downloads/connector/c/ "Download MySQL Connector:C 64-bit"
[python]: https://img.shields.io/badge/Python-Download-blue.svg "Download Python 2.7"
[python-link]: https://www.python.org/downloads/release/python-2715/ "Download Python"

After downloading and installing the above applications, to compile the servers run the following commands:

    git submodule update --init --recursive
    ./ci/msvc_install.bat
    cd build
    cmake ..
    cmake --build . --config Release

This will setup the build directories and compile. The compiled server will be in the bin folder created in the directory you ran cmake in (which should be the build folder).

After running for the first time, the servers will create a config file in the current directory that can be modified.

Server Configuration
--------------------
The servers can be confired using a `server.json` file. This file is automatically generated if any
server is started and no `server.json` file exists. It's recommended to generate this file before
continuing with the project setup. Simply start any of the servers to generate the file.

Client Setup
------------
The server is designed to work with the assets from the `ROSE_112_112NA` client. Using different
assets may cause issues due to incompatibility. The client is available to download
[here](https://mega.nz/#F!8AkG3TzJ?UQc3zZKb).

Run the installer and install the game to a desired location. While this project uses the assets
from that client it does not use the binaries. For full compatibility you will need to download
the latest client binaries from the project's [Releases Page](https://github.com/dev-osrose/osIROSE-new/releases)

After downloading the client files, copy them to the `ROSE_112_112NA` installation directory (default:
`C:\Program Files (x86)\TriggerSoft\Rose Online`) and overwrite the existing files.

 Connecting to the server requires using a special command from the command line, like so:

`trose.exe @TRIGGER_SOFT@ _server 127.0.0.1` (replace the IP if not running locally

For convenience, this command can be saved to a file with the extension `.cmd` in the same folder
as `trose.exe`. Then, the game can be launched by double clicking this file.

Database Setup
---------------
The server requires a [MySQL](https://mysql.com/) (or [MariaDB](https://mariadb.com/)) database server.

To setup the database, first create a new database (e.g. `osirose`). You will then need to execute the
`Database/osirose.sql` script to generate the database schema and the `Database/item_db.sql` to
populate the item database. This can be done using a database tool or from the command line like so: 
```
mysql -u<user> -p<password> <database_name> < Database/osirose.sql
mysql -u<user> -p<password> <database_name> < Database/item_db.sql
```

**NOTE**: The item table was generated using client v112_112na. If you are using a different client,
please generate your own table to match the information you need.

After setting up the database, two special users need to be created for the login and character
servers. They can be created by executing the following SQL commands:

```
call create_account("login", "098f6bcd4621d373cade4e832627b4f6");
call create_account("char", "098f6bcd4621d373cade4e832627b4f6");
update accounts set account_type="system" where username="login";
update accounts set account_type="system" where username="char";
```

The value `098f6bcd4621d373cade4e832627b4f6` is an MD5 hash of the default server passwords created
in the default `server.json` file. It should match the following sections:

```
    ...
    "charServer":  {
        ...
        "loginUser":         "login",
        "loginPassword":     "098f6bcd4621d373cade4e832627b4f6",
        ...
    },
    "mapServer":   {
        ...
        "charUser":     "char",
        "charPassword": "098f6bcd4621d373cade4e832627b4f6",
        ...
    },
    ...
```

To use your own password, generate an [MD5 hash](https://www.md5hashgenerator.com) of your password 
and update the accounts database and the `server.json` file.

Finally, we need to create an account to join the game with. Run the `create_account` procedure
again to create a user account, for example:

```
call create_account("admin", "21232f297a57a5a743894a0e4a801fc3" # Password is admin
```

Alternatively, automatic account creation can be enabled in the server config. This will automatically
create an account if a user attempts to login to an account that exist. This is recommended as it is
much more convenient than having to create password hashes and running sql commands. Simply, try to 
log in, fail, then try to login again with the same credentials!

To enable it modify `server.json` to enable `createAccountOnFail`:
```
"loginServer": {
    "createAccountOnFail": true,
    ...
},
```

Contributing
-----------------------

If you want to contribute, please read the [code of conduct](CODE_OF_CONDUCT.md). You can find some documentation about the source code [here](Documentation.md). Don't hesitate to contribute!
