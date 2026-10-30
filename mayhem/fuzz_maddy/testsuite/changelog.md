### 7.01
#### TBD
* Added filter box to package store.
* Poll manager for more stable comminucation while polling.
* Improved motor and app config write reliability.
* Less UI reloads when working with old firmware.
* Config caching to reduce the probability of accidently writing the default config.

### 7.00
#### 2026-05-15
* Firmwares are no longer included. They are now downloaded on demand.
* Drag and drop support of files from system.
* Added loganalysis source tab for local LBM logs and save them automatically when stopping the log.
* Added LispBM-page to mobile UI.
* Improved keyboard behavior on Android.
* Improved log analysis page thanks to https://github.com/vedderb/vesc_tool/pull/460

---

### 6.06
#### Released 2025-08-19
* Auto-save UI-state on regular intervals.
* Movable LispBM tabs and better tab behavior in general.
* Detect file system changes in script editor.
* Duplicate line shortcut in script editor.
* Use temperature convention on BMS page.
* Show Min, Max and Diff voltage on BMS page.
* Support building VESC Packages from QML description file.
* Filter package store based on rules from QML description.
* Added option to reduce LispBM file size by removing comments, spaces and imports.
* Disable motor commands after using the stop button or ESC key for 5 seconds (configurable in preferences).
* Show dialog with package description in mobile UI when installing from file.
* Better scrolling behavior in LispBM and QML console.

---

### 6.05
#### Released 2024-08-19
* Scripting-setting to select whether to upload the editor content or file content.
* Autocompletion and bracket matching completion improvements.
* Support for selecting multiple files when saving and deleting files from the log browser.
* Removed firmware and package tabs and replaced them with buttons.
* Shorter fault stop time for esk8 and balance in FOC wizard.
* Sanity-check motor parameters on write.
* Log analysis graph and table alignment fix.
* Disable BMS limits on balance vehicles in wizard.
* Ctrl+R to run selected code block LBM in repl.
* Script editor search improvements.
* Check if there are unsaved scripts before closing.
* Better lisp code read import handling.
* Added filter-box to example and recent lists in script editors.
* Limit output size in vesc and lbm terminal to prevent problems when printing too much.
* Use all 3 measured currents in sampled data instead of calculating one.
* Added support for loading sampled data CSV files.
* Always remove trailing whitespaces on save in CodeEditor.
* Comment block fix on windows and general improvements.
* Added custom config and QML cache for faster connecting and CAN device switching.
* Added option to reconnect to last CAN-device on connect.

---

### 6.02
#### Released 2023-03-12
* Improved experiment plot.
* Updated PPM and ADC mapping widgets.
* Better compatibility with old firmwares.
* LispBM
	* More examples
	* Rearranged UI
	* Added experiment plot
* Mobile custom package install support.
* Old firmware download RAM usage fix.
* Config backup/restore improvements.
* Automatically reload UI after installing and uninstalling packages.
* Automatically scan mobile CAN-list if empty.
* Changelogs updated and changed to markdown.
* Pixmap caching for faster start and config reload.
* Firmware archive added in mobile VESC Tool.
* Added display tool.
* QML upload bug fixes.

---

### 6.00
#### Released 2022-12-08
* Added support for compressed firmware upload.
* Reduce CPU usage by only plotting RT data when the plot is visible.
* Lisp scripting editor and integration.
	* Autocompletion.
	* Syntax highlighting.
	* Real-time binding, CPU and memory monitors.
	* Console for printing
	* Examples
* Added custom load code support to motor comparison tool.
* Added support for bitfield parameters.
* Added VESC UDP broadcast listener.
* Added VESC package creation and loading support.
* Added VESC package store. See https://github.com/vedderb/vesc_pkg
* Added TCP bridge.
* Custom config backup and restore.
* Connect screen on welcome page.
* Attempt at BLE-support on windows.
* Save and load support of configs on mobile.
* Custom config pages on mobile.
* Wizard and multisettings improvements when connected over CAN.

---

### 3.01
#### Released 2022-01-16
* Fixed simultaneous CAN FW upload when other devices (such as BMS) are on the CAN-bus.
* Fixed configuration backup and restore over CAN-bus.
* Added test version information to about dialog.
* Added scripting page
	* Syntax highlighting.
	* Recent files.
	* Example files.
	* Auto-completion tree.
	* Run in widget, window or full screen.
	* Debug print output.
	* Toggle line comment.
	* Auto-indentation of line or block.
	* Search, highlight and replace text.
* Only capture esc key for stopping the motor when connected.
* Fixed FOC detect all no can when connected over CAN.
* Increased motor and mosfet temperature limit editor range.
* Mobile RT display update. See: https://github.com/vedderb/vesc_tool/pull/115
* Added FOC wizard, setup data and profiles from mobile UI to desktop UI.
* Added usage page to FOC wizard.
* Added servo output control slider.
* Added support for loading custom UI from firmware.
* Dark theme.
* Mobile UI refactoring.
* Configurable data polling rates.
* Better DPI scaling.
* Configurable plot line thickness.
* Mobile:
	* New CAN forwarding bar.
	* Setting to disable screen rotation.
	* New RT Data page and gauges.
	* Statistics page.
* Upload bootloader before firmware if firmware has bootloader erase support.
* Added motor comparison tool.

---

### 3.00
#### Released 2021-01-11
* Focus for this release: External hardware and BMS support.
	* Added hw type setting.
	* Hide VESC settings when connected to other hardware.
	* Added BMS commands
* Other updates
	* Only allow uploading firmware to all VESCs when they have the same hardware version.
	* Added UDP server and client.
	* Added NRF52 UICR read and write support to SWDPROG page.
	* Realtime IMU and balance data