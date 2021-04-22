# FlashBack Diversity
<img src="Hardware/Pictures/PCB front.png" width="30%">
This project started out as a desire for a low cost diversity that would have good video quality. I started off looking into regular RX5808 video modules, but they have relatively low sensitivity. In my research process I found some "upgraded" rx5808 modules, and tried several different suppliers that claimed to sell these but sent me the regular RX5808 modules. I figured these would only be available to more high volume suppliers. Around this time TBS released a "Hardware Upgrade" module for their Fusion goggle receiver, and it caught my eye. It included TWO upgraded rx5808 modules, for the price of only $10. I ordered one to desolder these modules. See TBS Fusion [Hardware Upgrade](https://www.team-blacksheep.com/products/prod:fusion_hwupgrade)

During the time it took for the Fusion module to arrive, I changed my plan. I realized this module included a lot of parts I needed: The Goggle pins, the necessary connections for both rx5808 modules, and a handy little header connector to control everything. So instead of desoldering the modules, my plan was to design a control system that would mount onto this module. I followed the traces to check continuity and figured out the pinout, so now the fun part began! I started designing a PCB that could mount on top of the TBS hardware upgrade module and interface with the upgraded rx5808 modules and the video goggles.

I had to decide what I wanted the module to have, and came up with the following list.

### Features
- Diversity: Obviously. The idea of having two different antennas and selecting the one with best signal is a must.
- OLED display: A small display that could help with navigation and general graphic functions.
- Autoscan: Scan through all of the channels and select the one with highest RSSI
- Channel search: Scroll through all the 48 channels of the 5 bands (A,B,E,F & R)
- Band search: Scroll through all 8 channels of a selected band (A,B,E,F & R)
- Favorite channel selection: Quick select a favorite channel (Currently can only be modified through changing the code)
- Spectrum scanner: Show a bar graph with RSSI of all channels.

All of these must be navigatable through a simple interface of three buttons: up, down and enter. The enter button can be long pressed to go back a page. All of this must be displayed on the oled, there is nothing going through the goggle feed. This adds simplicity and switching speed between antennas. 

If you are interested in building this, I wrote a small tutorial here. For contact info check the end of this README.
## Build instructions

### 1. Purchase components

You might want to get started with ordering the PCB. The [Gerber files](Hardware/) can be found in the Hardware folder, which you can download and send to your favorite PCB manufacturer (I used JLCPCB but I'm sure any company will work fine, there are no tight design tolerances).

There is a [Bill of Materials](Hardware/) also in the Hardware folder of this repo with the list of required components. This list was taken for LCSC, but I believe these components can be easily found elsewhere. Please note that some of the components listed are for an inbuilt OLED display which was not implemented for this design. 

Additionaly, you need an 7 pin SPI OLED display module. The one used for this project is the 0.96" variant. An example of this display can be found [here](https://www.amazon.com/SSD1306-Display-MELIFE-Module-Arduino/dp/B087CW1YLK/ref=pd_sbs_1?pd_rd_w=rV6xU&pf_rd_p=de2765fe-65e5-4a88-aaad-a915dea49c67&pf_rd_r=RHG0ZNF5RSFNCNAR30E6&pd_rd_r=2ea41d08-5479-4dc0-b115-c3add2fa5a12&pd_rd_wg=ytx24&pd_rd_i=B087CW1YLK&psc=1).

The last thing you will need is one of the TBS Fusion hardware upgrade modules that were mentioned earlier (Important!).

### 2. Solder everything (except for the OLED!!)

Now you can solder all of the SMD components. Most of these are 0402 so you'll need a steady hand, but it's not impossible to do. Use the values for each component found in the "values front" and "values back" images in the [Pictures folder](Hardware/Pictures/). Double check polarity for the diodes and triple check polarity for the ICs! Take your time and check for shorts often.

### 3. Program the microcontroller

Programming the microcontroller is very straightforward. The PCB was designed to work as a STM32 Blue Pill, so you must select this option when programming it. It must be done in two steps:
1. Flash the Arduino bootloader using the SWD pins on the PCB, and one of those ST Link emulators like [these](https://www.amazon.com/st-link-v2/s?k=st-link+v2). There are plenty of tutorials for flashing the Arduino Bootloader into an STM32 Blue Pill out there already, so I'll save myself some time.
2. Now you can program the module through the micro USB port. Flash the [Flashback code](Code/Flashback/) as if it were a regular STM32 Blue Pill. Remember you must open all three files in the Flashback folder for the compiling to work!

### 4. Final assembly

You're almost there! Once you've programmed the microcontroller, you must solder the OLED module. After you've done this, you might want to check everything is working. Connect the PCB to power (using the micro USB connector will do) and navigate the menus. Up/Down buttons do just that, and the middle button does enter (short press) and back (long press).

If everything is working fine, you can remove the adhesive from the Fusion module and connect both PCBs together. Once this is done, the module must be calibrated.

### 5. Calibration

The process to calibrate the module is simple. Turn on a video transmitter (on any channel) about a meter away. Without connecting any antennas, power the module while pressing BOTH the up and down buttons. Keep holding them for a couple of seconds while the module runs the calibration routine (it will take a while). When the calibration is done, a splash screen containing the calibration values should appear for a couple of seconds. If these are not zero, everything works and you're ready to fly!

### Contact

For any questions, comments, etc. please don't hesitate to contact me!

Eduardo Lang

eduardolang90@gmail.com



##### Any use of this project implies agreement to the GNU General Public License
