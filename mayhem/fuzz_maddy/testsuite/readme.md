# VESC® Tool

This is the source code of VESC Tool. A pre-compiled binary of both the stable release as well as the development release packaged with all the matching firmware for all supported hardware can be downloaded at http://vesc-project.com/

The stable binary is available for **Linux**, **Windows**, **MacOS**, **Android** and **iOS**. The development binary is available for **Linux**, **Windows** and **Android** and is updated every few days.

All binaries can also be downloaded free-of-charge for all platforms except for iOS, which only is available via the Apple App Store as they do not allow any other distribution channel.

## Code Contribution, Distribution and Trademark Usage

VESC is a registered trademark of Benjamin Vedder. Read the [trademark policies](https://vesc-project.com/trademark_policies) for more information.

The "official" binary release of VESC Tool is done via VESC Project only, as that gives users a way to verify that releases, that use the registered VESC trademark, originate from the VESC Project. It is not ok to host a binary release on a different channel and use the VESC trademark for that release.

It is ok to use the github fork function to make contributions to the code. That is because 1) it is the most convenient way to make contributions and 2) the forked repository states clearly that it is a fork and points back to the main repository where the original code can be found. Further, it is easy to see what the code changes are from the forked repository compared to the main repository via github, but that information is lost in a binary release.

Forks of VESC Tool on github are not encouraged to provide a binary release in the repository. That is because there is no way to tell the final binary apart from the official release once downloaded. Further, packaging the firmware, which has to be done as an additional step from a different repository, also cannot be verified whether it is done correctly.

**Forks without branding**  

Because the topic came up, here are some words about forking VESC Tool and removing the branding.  

If you make a fork of VESC Tool and remove all traces of the VESC trademark you are not breaking the trademark policies, but we still do not encourage that. The reason is that such forks are 1) confusing to users and 2) divert users away from the VESC Project itself and take away the opportunity to learn about it and to make donations if they choose to. For example, the majority of VESC donations today come from VESC Tool downloads.  

If you see a missing feature and you want to put in some work and make that feature available, we would appreciate if you contribute that back to the main VESC repositories. That way there is only one consistent and compatible release for everyone that is managed by the main authors of the VESC code who make the vast majority of the development. It also gives the main authors, who are the most familiar with the code, a chance to review features to make sure that they are as safe as possible and don't break other parts of the functionality.

## Forking, porting or rewriting VESC® Tool — good practices

*Community write-up, not legal advice. The official [Trademark Policies](https://vesc-project.com/trademark_policies) and the [VESC Tool README](https://github.com/vedderb/vesc_tool) always prevail. When in doubt, ask: trademarks@vedder.se*

Rewriting VESC Tool in another language (Rust, Flutter, web…) or shipping a heavily modified fork touches two separate things:

- **The code** → copyright, governed by the GPL
- **The name and logos** → trademark, governed by Benjamin Vedder's trademark policy

The GPL gives you a lot of freedom with the code. It gives you **no** right to the VESC® name or logos. Keep both in mind.

### 0. First, consider contributing upstream

The README asks people to contribute missing features back to the main repos rather than splitting the user base. Custom hardware can be added to the official release by forking `vedderb/bldc` and opening a PR. If your goal can be reached that way, it's the best option for everyone.

### 1. The code: GPL-3.0-or-later

VESC Tool is licensed GPL-3.0-or-later. In most jurisdictions, translating it into another language or porting its logic file by file makes your project a derivative work. So:

- **License your project GPL-3.0-or-later** and ship the full LICENSE text.
- **Keep every existing copyright notice** (Benjamin Vedder and contributors) in the files you port, and add your own on top.
- **State clearly that it's modified**: what you changed, when, and from which upstream commit.
- **Publish the complete source** for every binary you distribute, build scripts included. A public repo works.
- **Show the legal notices in the UI**: an About screen with copyright, license, no-warranty notice and a link to the source.
- **Only use GPLv3-compatible dependencies** (MIT, Apache-2.0, BSD, MPL-2.0… are fine).
- **Bundled firmware** (`bldc`) is GPL too: same source obligations if you ship it.
- **Third-party code inside VESC Tool** (heatshrink, minimp3, QCustomPlot, maddy…) keeps its own license. Credit it if you port it.

A true clean-room reimplementation (written only from a protocol spec, without translating existing code) is a different situation, but it's hard to claim once you've worked from the source.

### 2. Naming

**VESC®** is a registered word mark of Benjamin Vedder. The policy says a mark can't be used as part of a product name, a domain name, a trade name or a company logo, and names confusingly similar to it are prohibited too.

- ❌ VESC Tool RS · RustVESC · VescTool NG · V-Tool · VSC Tool
- ✅ A name of your own that doesn't echo "VESC"
- Same for app store titles, listing titles, repo / package / crate names and domains. Even "MyApp for VESC" shouldn't be the title: compatibility goes in the description.
- A plain GitHub fork used to contribute upstream is fine; the README explicitly allows it.

###