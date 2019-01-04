# steins-gate-hrdarling-patch
Translation patch for Steins;Gate Hiyoku Renri no Darling/Darling of Loving Vows for PSP

![Screenshot of me having edited one of the dialog boxes](https://i.imgur.com/ltZuaTP.png)

Currently I have no translation aside from Zakobot's translations, which are hard to work with. I contacted Committee of Zero, who at the time of this writing is working on a patch for the PC version the the same game, but they aren't willing to release their translations before they're complete. Thus I am waiting for that to come out, and am putting what I currently have in this repo so I can resume work on that when it comes out.

I am putting a manual of how to setup your working directory and edit the game's text in the Github repo's [Wiki](https://github.com/compupro/steins-gate-hrdarling-patch/wiki), as well as a few insights into the structure of the game.

## Tools for working on the patch
If you want to work on the patch, you will need the following:
* Unmodified Steins;Gate Hiyoku Renri no Darling PSP iso, CRC32 hash `64A5777F`
* Atlas 1.11
* crifilesystem (should contain CpkFileBuilder.exe)
* UMDgen
* Python 3 (if you want to use util scripts)

If you're working on Windows like I am, you will also need:
* MinGW-w64
* msys

## Thanks
Major thanks to the following people:
* Riku on GBAtemp for creating the original proof-of-concept for changing the game's strings
* BASLQC on Github for keeping Riku's files on Github so Riku's work isn't lost forever
* HaselLoyance on Github for developing a bunch of the currently used tools, like Topographer, and for explaining to me the technical details and the process of modifying the original Steins;Gate PSP game.
