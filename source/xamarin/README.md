# Xamarin Adaptive Cards

## Android

### Prerequisites

1. Install Android Studio
1. Install [Java SDK 8](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html) 

### Build Android Studio project

First you need to build the Adaptive Cards Android project and generate the .AAR file (Android Archive) 

1. Follow the steps in the [Android README.md](../android/README.md)
1. You won't need to do rebuild this project again unless you change the Java source code

### Build Xamarin

1. Open `AdaptiveCards.Rendering.Xamarin.sln`
1. Set your **Solution Configuration** to **Android Debug**
1. F5

## iOS

### Prerequisites

1. Have a Mac handy 😉
1. Install Xcode >= 9

## Build XCode project

1. `cd source/ios/AdaptiveCards`
1. `xcodebuild -scheme AdaptiveCards-Universal build`
1. `unzip AdaptiveCards/AdaptiveCards.framework.zip`

## Build Xamarin

1. Open `AdaptiveCards.Rendering.Xamarin.sln`
1. Set your **Solution Configuration** to **iOS Debug**
1. F5