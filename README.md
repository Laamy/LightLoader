# Notes

the base ModLoader provides basic events like access to bedrocks WndProc aswell as an exit event and a load event for when the mod loader has finished loading mods
but the LightUt library still requires signatures to hook raknet and provide other basics

# How to install

drag the LightLoader.zip into your xbox games folder (Mine is C:\XboxGames\Minecraft for Windows\Content\)</br>
unzip it and then put mods in the mods folder

# How to configurate

All minecraft mods written for the mod loader *should* write its configs and other assets to its own folder in `.\Minecraft Bedrock\mods`</br>
to dumb it down press Win+R on your keyboard (Windows key) and enter this `%appdata%\Minecraft Bedrock\mods`</br>
mods only generate configurations when they've been run atleast once

# Images

I might scratch this but you get a funny splash screen while mods are being loaded</br>
<img width="732" height="474" alt="image" src="https://github.com/user-attachments/assets/201c9988-b8ba-4085-9d60-ff991ff18527" />

