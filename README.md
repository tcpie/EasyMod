EasyMod framework
=======

No-hassle modding in .Net

=======

You want to create a mod for a certain game. You want to be able to do some fancy stuff: *draw graphics*, *catch keypresses*, perhaps hook some functions. You're experienced in C# (or VB or any other .Net language), so you try to create a mod in one of those languages. But... you go nowhere. It turns out you need to dig deeper and need to write something in C/C++. You dislike these languages. While powerful, they are also hard.

No more! The EasyMod framework makes writing mods easy. All you need to do is inherit from the Script class, put your .dll in the "scripts" folder and your done. EasyMod takes care of loading your mod, provides an API for drawing and keypresses, and more.

The framework also comes with a developer-console, allowing your scripts to register console commands. The current implementation of the console is a EasyMod script itself, so if you don't like it, you can create your own! (have a look at the source in /Console/)

=======
Runtime dependencies
------------
EasyMod depends on the WinCore framework. You can grab the latest release from [this repository](https://github.com/tcpie/WinCore).

=======

Compile-time dependencies
(*Be sure to set your include and lib path accordingly!*)

* The WinCore framework (see above)
* The DirectX SDK

=======
Licence
-------
EasyMod is licensed under the GNU Lesser General Public License (GNU LGPL). This means any versions you make of EasyMod (example: forks) must also be licensed under the GNU LGPL and adhere to the license conditions. Your scripts relying on the EasyMod framework, however, **can be licensed under any license of your choosing**.

