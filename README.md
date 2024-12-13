[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/sitemule/noxDB)
# noxDB – Not Only XML…

noxDB is an opensource framework that makes it easy to work with XML, JSON and SQL with one single approach - from within RPG.

You can find examples and documentation on the [Sitemule documentation website](https://sitemule.github.io/noxdb/about).

### Installation

Installation of noxDB should be done with `git` and `gmake` (GNU Make), which are available via `yum` - you can read more about [yum here](https://bitbucket.org/ibmi/opensource/src/master/docs/yum/).

noxDB is a two step process. noxDB requires you to build from source, but this step has been totally automated for you. To install noxDB, you need to use the pase environment (with `ssh` for example) and with a couple of seconds you can have the project built. No need to download save files, upload them or restore them.

```
ssh my_ibm_i
mkdir /prj
cd /prj
git -c http.sslVerify=false clone https://github.com/sitemule/noxDB.git
cd noxDB
gmake
```

This will create:

* The `NOXDB` library
* `NOXDB/JSONXML` service program.
* `NOXDB/QRPGLEREF.XMLPARSER` for the XML noxDB API prototypes.
* `NOXDB/QRPGLEREF.JSONPARSER` for the JSON noxDB API prototypes.
* `NOXDB/NOXDB` binding directory, with the `JSONXML` object on it.


### Build the distribution.

When you have made the project in library NOXDB, you can create the release as a savefile

```
ssh my_ibm_i
cd /prj/noxDB
gmake clean release
```

### Further Reading

Want some further reading on noxDB?  See Andy Youens PowerWire article on [noxDB here.](https://powerwire.uk/noxdb-easily-use-json-in-rpg/)
