jQuery(document).ready(function ($) {
    
    var debounce = function(func, wait, immediate) {
     
        var timeout;
        return function() {
                var context = this, args = arguments;
                clearTimeout(timeout);
                timeout = setTimeout(function() {
                        timeout = null;
                        if (!immediate) func.apply(context, args);
                }, wait);
                if (immediate && !timeout) func.apply(context, args);
        };
    };
    
    var viewport = function () {
        var e = window, a = 'inner';
        if (!('innerWidth' in window )) {
            a = 'client';
            e = document.documentElement || document.body;
        }
        return { width : e[ a+'Width' ] , height : e[ a+'Height' ] };
    };
    
    
    var adjustWinH = debounce(function(){
        var w = viewport(),
            vpW = w.width;
        
        
        var sH = (vpW >= 768 )?(w.height - 65): 300;
        
            $('.sidebar').css({
                'height' : sH + 'px'
            });
            var center = map.getCenter();
            google.maps.event.trigger(map, "resize");
            map.setCenter(center);
        
        },200);
    
    
    $(window).on('resize.wind', adjustWinH).resize();
    
    $('.label-details').click(function(e) { e.preventDefault(); }).popover({
      trigger: 'click hover',
      html: true
    })

    $('#filterShops').keyup(function () {
        var rex = new RegExp($(this).val(), 'i');
        $('.col-seller-item').hide();
        $('.col-seller-item').filter(function () {
            return rex.test($(this).data('title'));
        }).show();
        
    });
    
    var pathToTheme = Drupal.settings.basePath + "sites/all/themes/" + Drupal.settings.ajaxPageState.theme;

    var customIcons = {
        inactive_profile: {
            icon: pathToTheme + '/images/markers/inactive_profile.png',
            zindex: 1
        },
        prospect_profile: {
            icon: pathToTheme + '/images/markers/inactive_profile.png',
            zindex: 2
        },
        customer_profile: {
            icon: pathToTheme + '/images/markers/customer_profile.png',
            zindex: 3
        },
        seller_profile: {
            icon: pathToTheme + '/images/markers/seller_profile.png',
            zindex: 4
        },
    };

    var map = new google.maps.Map(document.getElementById("directoryGoogleMap"), {
        center: new google.maps.LatLng(49.800855, 11.017640),
        zoom: 9,
        mapTypeId: 'roadmap'
    });

    var latlng = [];

    var infoWindow = new google.maps.InfoWindow;

    downloadUrl(Drupal.settings.basePath + 'rm-shop-participantxml', function(data) {
        var xml = data.responseXML;
        var markers = xml.documentElement.getElementsByTagName("marker");
        for (var i = 0; i < markers.length; i++) {
            var name = markers[i].getAttribute("name");
            var address = markers[i].getAttribute("address");
            var type = markers[i].getAttribute("type");
            var point = new google.maps.LatLng(
                parseFloat(markers[i].getAttribute("lat")),
                parseFloat(markers[i].getAttribute("lng")));
            if(type != 'inactive_profile') latlng.push(point);
            var html = "<b>" + name + "</b> <br/>" + address;
            var icon = customIcons[type] || {};
            var marker = new google.maps.Marker({
                map: map,
                position: point,
                icon: icon.icon,
                zIndex: icon.zindex
            });
            bindInfoWindow(marker, map, infoWindow, html);
        }
        google.maps.event.trigger(map, "resize");
        var latlngbounds = new google.maps.LatLngBounds();
        for (var i = 0; i < latlng.length; i++) {
            latlngbounds.extend(latlng[i]);
        }
        map.setCenter(latlngbounds.getCenter());
        map.fitBounds(latlngbounds);
    });

    function downloadUrl(url,callback) {
        var request = window.ActiveXObject ?
            new ActiveXObject('Microsoft.XMLHTTP') :
            new XMLHttpRequest;

        request.onreadystatechange = function() {
            if (request.readyState == 4) {
                request.onreadystatechange = doNothing;
                callback(request, request.status);
            }
        };

        request.open('GET', url, true);
        request.send(null);
    }

    function bindInfoWindow(marker, map, infoWindow, html) {
        google.maps.event.addListener(marker, 'click', function() {
            infoWindow.setContent(html);
            infoWindow.open(map, marker);
        });
    }

    function doNothing() {}
    
    
});