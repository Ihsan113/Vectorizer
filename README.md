# ImageToEPS v0.4.0

Clean Android Image → EPS MVP.

This archive intentionally contains NO `.git` directory, build cache, or old workflows.

The GitHub workflow does NOT use `android-actions/setup-android@v3`, and contains no `packages: tools` setting and no `sdkmanager tools` command.

It uses the Android SDK already present on the GitHub-hosted runner and installs only:
- platform-tools
- platforms;android-35
- build-tools;35.0.0
- ndk;27.2.12479018
- cmake;3.22.1

The current native engine produces real EPS vector rectangles. OpenGL ES 3 is linked for a future GPU preprocessing stage; the current vectorizer is not falsely represented as GPU-accelerated.
