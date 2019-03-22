function onNameChanged(name) {
    var shouldShow = name.length > 2;
    host.ToggleVisibility('successText', shouldShow);
}