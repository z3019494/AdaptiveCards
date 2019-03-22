function updateTime() {
    const Http = new XMLHttpRequest();
    const url='http://api.geonames.org/timezoneJSON?formatted=true&lat=47.01&lng=10.2&username=demo&style=full';
    Http.open("GET", url);
    Http.send();
    Http.onreadystatechange=(e)=>{
        host.setText('currentTime', e.responseText);
    }
}

updateTime();