# ESM Firmware for T Watch 2020 V2

A Firmware to facilitate studies using in-situ self-reports with the T-Watch 2020 V2 (more information on [their repository](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)).

This firmware allows to setup questionnaires and notifications using a simple JSON configuration file. The watch does not connect to any networks. It will automatically read configuration files stored on an inserted micro-SD card, and store responses and logs as CSV on the micro-SD card as well, making data transfer easy.

For an easy way to configure a study configuration file see the configuration application: https://github.com/KL-Psychological-Methodology/ESM-Firmware-for-T-Watch-2020-V2-Configuration-App

# Questionnaire Features

The firmware allows to define multiple questionnaires, each with multiple items. Questionnaires can be set to always be selectable by participants (e.g., for when they should be filled out in response to a real-life event). They can also be made accessible via notifications that are triggered via a predefined schedule (see Notification Features).

Each item has an item text, a type-specific way of inputting a response, and a *Continue* button that advances the questionnaire (or completes it on the last item). The firmware features the following item types to be used in questionnaires:

* **Likert Scale**: A scale with multiple points, with text labels at the scale ends. The number of points can be configured.
* **Visual Analogue Scale**: A continuous scale, with text labels at the scare ends.
* **Options**: Multiple options in a scrolling list (similar to a dropdown list).
* **Numeric**: A way to input numbers.
* **Text**: A pseudo-item without input. Allows to display more text on the screen, e.g., to give context for the following items.

Questionnaires can optionally define a range of items that should be shuffled (e.g., have the first item with explanatory text always display first, but present all other items in random order).

# Notification Features

Multiple notifications can be defined for each questionnaire. A triggered notification will wake the device, alert the participant via the vibration motor and display a notification message that allows the participant to proceed to the associated questionnaire. When defining notification trigger times, the following options are available:

* **Fixed time or random interval**: Notifications can either trigger at set times, or can be defined to occur randomly within a set time interval (defined by a start time and inderval duration)
* **Expiry Time**: If the watch is woken up from sleep mode before an unanswered notification is expired, it will be displayed again (this is usiful if the participant did not notice it right away, or could not respond to the initial notification).
* **Reminder**: Unanswered notifications can be set to be elicited again for several times after they would otherwise expire.
