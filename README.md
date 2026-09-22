# ImageToEPS 0.3.0

Clean Android Image → EPS MVP.

## GitHub Actions

The workflow contains NO `sdkmanager tools` command.

It installs only:
- platform-tools
- platforms;android-35
- build-tools;35.0.0
- ndk;27.2.12479018
- cmake;3.22.1

Then Gradle builds `assembleDebug`.

## Runtime

The native C++ engine creates actual EPS vector paths using vector rectangles.

OpenGL ES 3 is linked and reserved for the next GPU preprocessing implementation.
This MVP does not claim that the CPU vectorizer is already GPU accelerated.

## Output

APK artifact:
`ImageToEPS-debug`
