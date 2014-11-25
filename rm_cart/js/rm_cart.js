jQuery(document).ready(function ($) {
    
    var rmCart = function() {
        this.$cartContainer = $('.cart-container');
        this.$cart = $('#cart');
        this.cartH = this.$cart.height();
        this.$add2CartButtons = $('.add2Cart');
        this.CART_ADD_STEP = 1;
        this.DELETE_ITEM_CLASS = '.delete-item';
        this.STEPPER_CONTROLS_CLASS = '.stepper-control';
        this.STEPPER_QTY_INPUT_CLASS ='.stepper-qty';
        this.LOADER_CLASS_NAME ='loader';
        this.$win = $(window);
        this.pageOffsetY = 65;
        this.adminbarOffset = 1;
    };
    
    rmCart.prototype = {
        
        init : function() {
            var _self = this;
         
         // if(!_self.cartTooHigh()) {
         //     _self.addAffix();
         // };
         _self.setEventHandlers();
        },
        
        setEventHandlers : function() {
            var _self = this;
          
            // Add2Cart
            _self.$add2CartButtons.on('click.add2Cart', function(e) {
                e.preventDefault();
                var $el = $(this),
                    data = _self.getItemData($el);
                    
                _self.addToCart(data);
            });
            
            //Remove
            _self.$cartContainer.on('click.removeFromCart', '' + _self.DELETE_ITEM_CLASS + '', function(e){
                e.preventDefault();
                var $el = $(this),
                    pid = $el.attr('href'),
                    $cont = $el.parents('.cart-item');
                    
                    _self.addLoader($cont);
                    _self.removeItemFromCart(pid);
            });
            
            // Stepper + und -
            _self.$cartContainer.on('click.stepperChange', '' + _self.STEPPER_CONTROLS_CLASS + '', function(e){
                e.preventDefault();
                var $el = $(this),
                   // keepOn = true,
                    $cont  =  $el.parents('.cart-item'),
                    $qty = $el.closest('.stepper').find('input.stepper-qty'),
                    qtyVal = parseInt($qty.val(),10),
                    items =  qtyVal + (1 * $el.data('operation'));
                    
                     
                    var data = {
                            offerid :  parseInt($cont.data('offerid'),10),
                            variation : parseInt($cont.data('variation'),10),
                            tradingunit : parseInt($cont.data('tradingunit'),10),
                            amount : items,
                            add : 0
                        }
                    _self.addLoader($cont);
                    _self.addToCart(data);
                
            });

             // Stepper Input Focus
            _self.$cartContainer.on('focus.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS + '', function(e){
                var $el = $(this);
                $el.mouseup(function(e) { return false; });
                $el.select();
                inputValCache = $el.val();
            });
            
            // Stepper Input Change
            _self.$cartContainer.on('change.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS + '', function(e){
                var $el = $(this),
                    inputVal = $el.val(),
                    numeric = /^[0-9]+$/.test(inputVal),
                    maxItems = parseInt($el.attr('max'),10);
                    
                if ( (numeric) && (inputVal <= maxItems)  ) {
                   
                    var $cont  =  $el.parents('.cart-item'),
                        offerid = $cont.data('offerid');
                        
                    var data = {
                        offerid :  parseInt($cont.data('offerid'),10),
                        variation : parseInt($cont.data('variation'),10),
                        tradingunit : parseInt($cont.data('tradingunit'),10),
                        amount : inputVal,
                        add : 0
                    }
                    _self.addLoader($cont);
                    _self.addToCart(data);
                    
                } else {
                    
                    $el.val(inputValCache) ;
                }
            });
        },
        
        getItemData : function($el){
            var _self = this;
                tu = $el.parent().siblings('.product-infos').find('.label-area .price').data('tradingunit'),
                item_data = {
                    offerid : parseInt($el.data('offerid'),10),
                    variation : parseInt($el.data('variation'),10),
                    tradingunit : parseInt(tu,10),
                    amount : _self.CART_ADD_STEP,
                    add : 1
                };
                
            return item_data;
        },
        
        addToCart : function(item_data, uid) {
            var _self = this,
                callback_url = Drupal.settings.basePath + 'addtocart/' + item_data.offerid + '/' + item_data.variation + '/' + item_data.tradingunit + '/' + item_data.amount + '/' + item_data.add;
                
                data = {};
            if(typeof uid !== 'undefined') {
                data['uid'] = uid;
            }
            
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data
            }).done(function() {
                _self.updateCart();
            }); 
        },
        // aktualisierte Darstellung
        updateCart : function(){
            var _self = this,
                data = {};
                
            data['module'] = 'rm_cart';
            data['block'] = 'rm_cart_block';
            var callback_url = Drupal.settings.basePath + 'invokeblock';
            
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data,
                success: function(data) {
                    console.info(data);
                    _self.$cartContainer.html(data);
                  _self.cartTooHigh() ;
                }
            });
        },
        
        addLoader : function ($el) {
            var _self = this;
            $el.append('<div class="' + _self.LOADER_CLASS_NAME + '""></div>');
        },
        
        removeLoader : function($el) {
            var _self = this;
            $el.find(_self.LOADER_CLASS_NAME).remove();
        },
    
        removeItemFromCart : function(pid,uid) {
            var _self = this,
                callback_url = Drupal.settings.basePath + 'removefromcart/' + pid,
                data = {};
                
            if(typeof uid !== 'undefined') {
                data['uid'] = uid;
            }
                 
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data
                
            }).done(function() {
             
                _self.updateCart();
            }); 
        },
        
        cartTooHigh : function(){
            var _self = this,
                winH = _self.$win.height()- _self.pageOffsetY,
                cartH = $('#cart').outerHeight();
            
            return (cartH > winH) ? true:false;
        },
        
        addAffix : function(){
            var _self = this,
                offset = _self.$cartContainer.offset().top - _self.pageOffsetY;
            _self.$cartContainer.affix({
                    'offset' : _self.adminbarOffset
            });
        },
        
        removeAffix : function() {
             var _self = this;
             _self.$win.off('.affix');
             _self.$cartContainer
                .removeClass("affix affix-top affix-bottom")
                .removeData("bs.affix");
        }
    }
     
    var cart = new rmCart();
        cart.init();
        
    $( '.dropdown-menu li' ).on( 'click.dropdown', function() {
 
        var $el = $(this),
            newContent = $el.find('a').html();
            
        $el.closest( '.btn-group' )
           .find('.label-area').html(newContent)
              .end()
           .children( '.dropdown-toggle' ).dropdown( 'toggle' );
        $el.siblings().removeClass('hidden');
        $el.addClass('hidden');
        
        return false;
 
    });
    
    var Map = function(){
        
    }
    
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

    $('#pickupModalToggle').on('click.pickupModal', function(){
         
        $('#pickupModal').modal();
    });
 
 $('#pickupModal').on('shown.bs.modal', function() {
     var pathToTheme = Drupal.settings.basePath + "sites/all/themes/" + Drupal.settings.ajaxPageState.theme;
        var pickupIcon = pathToTheme + '/images/markers/pickup_icon.png';
        var infoWindow = new google.maps.InfoWindow;
        var latlng = [];
   
    var map = new google.maps.Map(document.getElementById("pickupMap"), {
            maxZoom: 15,
            mapTypeId: 'roadmap'
        });
    
      
        downloadUrl(Drupal.settings.basePath + 'rm-shop-spotxml/' + Drupal.settings.suid, function(data) {
        var xml = data.responseXML;
        var markers = xml.documentElement.getElementsByTagName("marker");
        
        for (var i = 0; i < markers.length; i++) {
             
            var address = markers[i].getAttribute("address");
            var id = markers[i].getAttribute("nid");
            
            var point = new google.maps.LatLng(
                parseFloat(markers[i].getAttribute("lat")),
                parseFloat(markers[i].getAttribute("lng"))
                );
            
            latlng.push(point)
                
            var html = "<b>" + address + "</b>";
            
            var marker = new google.maps.Marker({
                map: map,
                position: point,
                icon: pickupIcon,
                 
            });
            bindInfoWindow(marker, map, infoWindow, html);
        }
        
        var latlngbounds = new google.maps.LatLngBounds();
        for (var i = 0; i < latlng.length; i++) {
            latlngbounds.extend(latlng[i]);
        }
        map.setCenter(latlngbounds.getCenter());
        map.fitBounds(latlngbounds);
    }); 
  
});
 
 
 function doNothing() {
    
 }

var $sidebar = $('#flexfix-sidebar');
var $cartToggle = $('#cart-toggle');

$cartToggle.on('click.cartToggle', function() {
    $(this).toggleClass('active');
    $sidebar.toggleClass('active');
});
                
});
