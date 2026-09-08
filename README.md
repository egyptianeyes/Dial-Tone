# Dial-Tone

> **NOT SUITABLE FOR PRODUCTION.**

Dial-Tone, The Administrator's Softphone, is a native Windows SIP softphone derived from
[MicroSIP](https://www.microsip.org/). MicroSIP remains the foundation for SIP
accounts, registration, calling and media. Dial-Tone develops that foundation for PBX
administration and troubleshooting without introducing a separate SIP stack.

Its principal additions are pre-call readiness qualification, an audible local dial
tone, Call Trace, Call Notes and records, live connected-identity handling, PBX controls
and an administrator-oriented Windows interface.

## Screenshot

> A current Dial-Tone application screenshot has not yet been added to the repository.

## Development status and builds

Dial-Tone is under active development. The repository provides source code and a
GitHub Actions workflow that builds a 32-bit Windows Release executable. There is no
installer or separately documented stable release channel at present.

Build artefacts are available from
[GitHub Actions](https://github.com/egyptianeyes/Dial-Tone/actions/workflows/build-windows.yml),
subject to GitHub's retention policy. Development executables are currently unsigned.
Windows may therefore apply download-reputation checks or display a security warning.
Verify the source revision and build provenance before running an executable. Do not
disable Windows security features solely to run Dial-Tone.

## Requirements and compatibility

The reference CI build is a Win32/x86 Release build using Microsoft Visual C++ v143,
Windows SDK selection `10.0` and WebView2 native dependency `1.0.3351.48`. The embedded
manifest declares Windows 7, 8, 8.1 and 10 compatibility, but not every Dial-Tone
change is continuously tested on every listed Windows release.

A SIP account, access to a compatible SIP registrar or PBX, and working audio input
and output devices are required for normal use.

## Voice calling

Dial-Tone retains MicroSIP's established SIP and audio-calling foundation:

- multiple SIP accounts, authentication and registration
- incoming and outgoing audio calls
- input, output and ringing device selection
- audio codec selection and priorities
- DTMF, hold and resume
- blind and attended transfer
- conference calling and audio mixing
- call forwarding and Do Not Disturb
- voicemail and Message Waiting Indication
- manual and automatic call recording
- auto-answer and automatic hang-up
- local contacts, presence and call history

Dial-Tone extends selected parts of this workflow, particularly connected identity,
Call Forward All, diagnostics and account presentation. Registration, call state and
audio media continue to use MicroSIP and PJSUA/PJProject.

## Dial-Tone readiness

The readiness control performs a pre-call check against the selected account and its
configured SIP service. It opens Call Trace and checks:

- an enabled account, running PJSUA state and valid account ID
- registration state and the configured registrar or server
- the selected UDP, TCP or TLS transport and its availability
- the selected playback and capture devices
- successful sound-device opening
- the presence of at least one enabled audio codec
- SIP OPTIONS responsiveness, including 401/407 authentication handling
- the final SIP response, responding endpoint, actual transport and OPTIONS RTT

`READY` means the required local checks succeeded, the account has a successful
registration and OPTIONS received a successful response. An account configured
without a registrar is deliberately classified as `DEGRADED`; the check can still use
the configured server for OPTIONS, but a successful response does not promote that
account to `READY`. Other degraded results include a final non-success response below
SIP 500 or a successful OPTIONS response over 1,000 ms. `NOT READY` identifies a hard
failure such as an invalid account, registration failure, unavailable transport or
audio device, no enabled audio codec, authentication failure, server error or timeout.

On success, Dial-Tone can start the selected locally generated tone. Qualification and
tone state are cancelled when a target or first digit is entered, a call starts, the
account, network or audio devices change, PJSUA stops, or the application is hidden.
A recent successful qualification is associated with a subsequent outgoing call only
when that call starts on the same account within 60 seconds.

The check establishes local account, registration, transport, audio-device and codec
readiness together with registrar or server responsiveness. It does not establish:

- PBX dial-plan routing or destination existence
- PSTN or carrier reachability
- successful INVITE routing
- remote RTP establishment or NAT traversal
- bidirectional speech or real-call audio quality

## Audible dial tone and presets

The audible dial tone is generated locally after successful qualification. It is not
audio supplied by the PBX. Settings provides these presets:

- `350+440 Hz`, the default
- `400 Hz`
- `425 Hz`
- `440 Hz`
- `450 Hz`

The **Sample** control plays the selected preset through the configured output device.
Dialling, call activity, cancellation and readiness invalidation stop the tone.

## Call workspace

The lower workspace combines current-call diagnostics, notes and retained records.
Starting readiness selects Call Trace. A real answered call selects Call Notes once,
while still allowing the user to change workspace during the call.

### Call Trace

Call Trace can include:

- readiness results
- call creation and incoming or outgoing direction
- account, server and SIP Call-ID
- local and remote identities and the remote contact URI
- SIP and call state, including final SIP status
- connected-identity transitions
- negotiated audio and codec information
- remote RTP endpoint
- RTP receive/transmit statistics, packet loss, jitter and RTT

The Rich Edit display applies semantic colour, hierarchy and wrapping. The
authoritative trace remains plain text, so copied and saved output contains no RTF,
HTML or display-only formatting.

### Call Notes and records

Call Notes provides an editor for the current call. When a real incoming or outgoing
call first reaches PJSIP's `PJSIP_INV_STATE_CONFIRMED` state, Dial-Tone selects Notes
once. Ringing, early media, failed or cancelled calls, SIP OPTIONS and readiness checks
do not trigger answered-call behaviour.

Changed, meaningful notes are saved through the normal record mechanism when an
answered call ends. Manual saving updates the saved state, so call end does not create
another record unless the note subsequently changes. Unchanged or empty notes do not
create pointless duplicate records. Replacing unsaved current notes retains the save,
discard or cancel protection.

Current and historical Trace and Note records can be browsed. Records are UTF-8 text
with a byte-order mark and include caller ID, start time, duration, username and SIP
Call-ID. The five newest matching records are indexed in the workspace; older files
remain in storage.

## PBX operations

### Connected identity during attended transfer

MicroSIP already reads connected identity from `P-Asserted-Identity`, falling back to
`Remote-Party-ID`. Dial-Tone extends this handling so a genuine identity change can
immediately update the existing live call.

The update is associated by PJSUA call ID, preserving the same call and call tab. The
cached identity is refreshed together with the caller name and number, active call
presentation, dialler and relevant history surfaces. Call Trace also records the
transition. After an attended transfer, this lets the receiving user see the connected
or original party identity supplied by the PBX rather than retaining the consultation
party's identity. Dial-Tone cannot recover an identity the PBX does not send.

### Call Forward All

Forwarding mechanics are inherited from MicroSIP. Dial-Tone adds a permanent **CFW**
control and inline **Forward Destination** field. Destination editing has explicit
commit and abandon behaviour and remains synchronised with Settings. When enabled,
incoming calls are redirected client-side through the existing forwarding mechanism.
Applicable forwarding events and results appear in Call Trace.

### PBX test shortcuts

- **Spk Clock**: `*60`
- **Echo Test**: `*43`

These controls use the normal call path. Availability depends on the configured PBX
dial plan.

## Contacts, directories and history

Dial-Tone retains MicroSIP's local contacts, presence subscriptions and ordinary call
history. Contacts support CSV import and export. Call history retains filtering,
deletion and export.

The remote users directory accepts CSV, JSON, native `contacts` XML, Yealink
`YealinkIPPhoneBook` XML, and compatible XML using `entry` or `DirectoryEntry`
records. Directory refresh, silent updates and presence remain part of the inherited
MicroSIP architecture.

## Administrator interface

### Account switching

The Phone view displays the active account or extension and provides a front-panel
sidecar for selecting another account. It reuses MicroSIP's account-switching path and
does not create a separate account store.

### Dark mode

Dial-Tone adds persisted, reversible dark mode across the main dialler, keypad, status
presentation, workspace editors and controls, Contacts and Logs.

### Dynamic workspace widths

Phone, Logs and Contacts use view-appropriate widths. Logs and Contacts calculate
their required width from current live list-column geometry. Returning to Phone
restores its compact width. View switching does not change the window height.

### Window and docking behaviour

The main window follows the current monitor's work-area height and uses a fixed width
for the active view. It can dock at the left or right edge through Windows AppBar,
reserving desktop workspace. The reservation is released while dragging and reapplied
when docked again. Placement is clamped per monitor, including multi-monitor use.
Floating width changes attempt to preserve the window centre and keep it visible.

Dial-Tone retains minimise and close controls, together with native title display and
caption dragging. Ordinary resizing and maximising are deliberately unavailable.

### Branding

Dial-Tone carries Egyptian Eyes branding while retaining visible MicroSIP attribution.
The Branding control can replace the displayed logo and web address. Restoring the
default returns to the bundled Dial-Tone branding.

## Differences from upstream MicroSIP

Current Dial-Tone is deliberately audio-only. These upstream capabilities are disabled
or removed from the current user-facing product:

- video calls, video answer, devices, codecs and preview
- user-facing SIP chat and messaging entry points
- automatic and manual MicroSIP upstream update checks
- ordinary resizable and maximisable window behaviour

Transfer, conference calling, recording, DND, voicemail, presence and remote
directories remain available. Disabling the updater also prevents a downstream build
from offering to replace itself with an upstream MicroSIP executable.

## Built on MicroSIP

Dial-Tone is downstream MicroSIP development. MicroSIP provides the native Windows
application architecture, account and registration handling, call-state management,
PJSIP/PJProject integration, media negotiation, codecs, contacts, history,
configuration persistence and underlying softphone controls.

Derived source files retain their MicroSIP copyright and GNU GPL notices. The
application displays:

```text
Powered by MicroSIP - SIP Softphone for Windows
```

with a link to the [MicroSIP website](https://www.microsip.org/). The retained source
directory name reflects the upstream archive layout. Dial-Tone's version is defined
separately in `const.h` and follows the MicroSIP-derived version with a downstream
`-dt` revision, for example `3.22.12-dt0`.

MicroSIP is independent of Dial-Tone and Egyptian Eyes. Dial-Tone is not an official
MicroSIP release and is not endorsed by or affiliated with the MicroSIP project.

## Installation and quick start

The repository currently produces a standalone executable rather than an installer.

1. Obtain it from a trusted GitHub Actions run or build it from source.
2. Keep it in a writable application folder if using portable mode.
3. Start Dial-Tone and select **Add Account...** from the main menu.
4. Enter the registrar, proxy, username, domain, password and transport information
   supplied by the PBX administrator.
5. Save the account and confirm registration.
6. Run the Dial-Tone readiness check before placing a call.

## Building

### Reference CI build

`.github/workflows/build-windows.yml` runs on GitHub's mutable `windows-2022` image,
so it is not a bit-for-bit reproducible environment. Native source revisions are
pinned, while the runner, installed compiler revision and selected SDK may change.

- Platform: `Win32`
- Configuration: `Release`
- Platform toolset: `v143`
- Windows SDK selection: `10.0`
- PJProject: `7de6e686fee1642f5443a3f39f0e5ad5e478a117`
- Opus: `ddbe48383984d56acd9e1ab6a090c54ca6b735a6`
- WebView2: `Microsoft.Web.WebView2` version `1.0.3351.48`

PJProject's `pjlib/include/pj/config_site.h` is generated with:

```c
#define PJMEDIA_HAS_OPUS_CODEC 1
#define PJMEDIA_HAS_VIDEO 0
```

This is an intentional audio-only configuration. Commit `2b682b7` disabled PJProject
video and removed MicroSIP's `_GLOBAL_VIDEO` switch as one coordinated change. Opus
and the established SIP and audio calling paths remain enabled.

### Build sequence

CI restores WebView2, checks out and builds the pinned Opus and PJProject revisions,
generates `config_site.h`, builds the inherited MicroSIP Visual Studio project, renames
the output and uploads the executable and associated outputs.

The principal build commands are:

```powershell
msbuild "$buildRoot\pjproject-vs14.sln" /m /t:libpjproject `
  /p:Configuration=Release-Static /p:Platform=Win32 `
  /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0

msbuild "$projectDir\microsip.vcxproj" /m /t:Build `
  /p:Configuration=Release /p:Platform=Win32 `
  /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0 `
  "/p:OutDir=$artifactDir/" "/p:IntDir=$intermediateDir/"
```

Use the complete workflow when reproducing its directory and environment preparation.
The project first writes `microsip.exe`; CI renames it to:

```text
Dial-Tone_<MicroSIP-version>-<Dial-Tone-revision>.exe
```

The current definitions produce `Dial-Tone_3.22.12-dt0.exe`.

## Configuration and local storage

Dial-Tone inherits MicroSIP's portable and installed storage behaviour. Portable
configuration, contacts and logs are stored beside the executable, with an INI name
derived from the executable filename. Installed-mode paths are:

```text
%APPDATA%\MicroSIP\
%LOCALAPPDATA%\MicroSIP\
```

Dial-Tone-specific records and custom branding are stored under:

```text
%APPDATA%\MicroSIP\freepbxUK\CallRecords\
%APPDATA%\MicroSIP\freepbxUK\Branding\
```

These paths use roaming application data directly, including when the main
configuration operates in portable mode. Configuration and records can contain SIP
identities, network details and personal data. Protect them and redact sensitive data
before sharing diagnostics.

## Development and contributions

Changes should:

- preserve upstream copyright and licence notices
- keep SIP and media behaviour in established MicroSIP/PJSIP paths unless required
- keep diagnostic presentation separate from authoritative diagnostic data
- preserve plain-text copying and storage
- avoid unnecessary runtime dependencies
- support light and dark Windows presentation
- document changed build inputs and downstream behaviour

Use [GitHub Issues](https://github.com/egyptianeyes/Dial-Tone/issues) for reproducible
defects and focused requests. Include the Dial-Tone and Windows versions, relevant call
state and sanitised trace or crash information. Never publish SIP passwords or
unredacted private data.

### AI-assisted contributions

AI tools may assist investigation, implementation, testing, documentation and review.
The human contributor remains responsible, and AI tools must not be co-authors.
AI-assisted commits should disclose the tool and model:

```text
Assisted-by: AGENT_NAME:MODEL_VERSION
```

For example:

```text
Assisted-by: GitHub-Copilot:gpt-5.6-sol
```

## Acknowledgements

Dial-Tone depends on the
[MicroSIP project](https://www.microsip.org/source),
[PJProject/PJSIP](https://www.pjsip.org/) and
[Opus](https://opus-codec.org/). Their copyright notices and licence terms continue
to apply.

## Author

[@kierknoby](https://github.com/kierknoby), Kieran Knowles-Byrne // [Egyptian Eyes](https://github.com/EgyptianEyes)

## Licence and assets

Inherited MicroSIP source headers permit redistribution and modification under the GNU
General Public License as published by the Free Software Foundation, version 2 or, at
the recipient's option, any later version. Those headers and upstream notices remain
in the derived files.

A complete repository-level `LICENSE` file is not currently present. This remains a
publication blocker. The README is not a substitute for the applicable licence text
and notices. The staged CMarkup-to-XmlLite implementation is intended to remove
CMarkup from active application source, but does not complete the repository-wide
licensing and notices review.

The Dial-Tone and Egyptian Eyes names, logos, artwork and trademarks are distinct from
the GPL-covered program source. The repository does not yet contain a separate asset
licence or complete notice covering the bundled Dial-Tone logo and all inherited image
resources. No additional asset rights should be inferred from the software licence.
