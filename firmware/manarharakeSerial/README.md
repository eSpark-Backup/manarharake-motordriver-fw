## **How to Add Teensy Board and setup Library in Arduino IDE**

Teensy boards, along with specific libraries like FreeRTOS, can greatly enhance your Arduino projects. Here’s how to add Teensy board support and the FreeRTOS library:

### Step 1: Open Arduino IDE

1. Launch Arduino IDE on your computer.

### Step 2: Open Preferences

1. Go to `File > Preferences` (on Windows/Linux) or `Arduino > Preferences` (on macOS).

### Step 3: Add Teensyduino URL

1. In the "Additional Board Manager URLs" field, add the following URL:
   ```
   https://www.pjrc.com/teensy/td_download.html
   ```

### Step 4: Install Teensyduino

1. Go to `Tools > Board > Boards Manager...`.
2. Wait for the Boards Manager to load.
3. In the search bar, type "Teensy".
4. Click on "Teensy (for Arduino IDE 2.0.4 or later)" when it appears in the list.
5. Click the "Install" button.
6. Wait for the installation to complete.

### Step 5: Select Teensy Board

1. Go to `Tools > Board`.
2. You should now see a list of Teensy boards available.
3. Select the Teensy board model you have (e.g., Teensy 3.2, Teensy 4.0).

### Step 6: Configure the Board Settings

1. Select the appropriate settings for your Teensy board, such as the `USB type : Serial`, CPU speed, etc. These settings can be found under `Tools` menu.

### Step 7: Add FreeRTOS Library

1. Download the FreeRTOS library from [freertos-teensy-11.0.1_v1](https://github.com/tsandmann/freertos-teensy/archive/refs/tags/v11.0.1_v1.zip).
2. Save the library ZIP file to a location on your computer where you can easily find it.

### Step 8: Install the Library

1. Go to `Sketch > Include Library > Add .ZIP Library...`.
2. Navigate to the location where you saved the FreeRTOS library ZIP file.
3. Select the ZIP file and click "Open".

### Step 9: Verify Installation

1. Once you’ve added the library, you can verify if it was installed correctly.
2. Go to `Sketch > Include Library`. You should see "FreeRTOS-Teensy" or similar listed here.

Now the code should be able to compile!