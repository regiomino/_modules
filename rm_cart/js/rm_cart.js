jQuery(document).ready(function ($) {

var RC = RC || {};
window.RC = RC;  

RC.init = function () {
    var _self = this;

    _self.products.init();
}

/*
Products
*/

RC.products = {};
RC.products.$cont = $('#product-grid-container');
RC.products.$items = $('.product-item', RC.products.$cont);
RC.products.offers = {};

RC.products.init = function(){
    var _self = this;
    _self.buildProducts();
};

RC.products.buildProducts = function(){
    var _self = this;

    RC.products.$items.each(function(){
        var $el  = $(this),
            offerid = $el.data('offerid');
            variation = $el.data('variation'),
            visibletradingunit = $el.data('visibletradingunit'),
            currentamount = $el.data('currentamount');

            _self.offers[ offerid  ] = new _self.Item(offerid, variation, visibletradingunit, currentamount);
    });
};

RC.products.Item = function( offerid, variation, visibletradingunit, currentamount ) {

    var binder = new DataBinder(offerid);

    var item = {
        props : {
            offerid : offerid,
            variation : variation,
            visibletradingunit : visibletradingunit,
            currentamount : currentamount
        },

        set : function(attr_name, val) {
            var _self = this;
            _self.props[ attr_name ] = val;
           /* //Publish
            binder.trigger(offerid + ":change", [attr_name, val, _self ] );*/
        },

        get : function(attr_name) {
            return this.props[ attr_name ];
        },

        getAll : function() {
            return this.props;
        }
    };

    //Subscribe
   /* binder.on( offerid + ":change", function(evt, attr_name, new_val, initiator ) {
        if ( initiator !== item) {
            item.set( attr_name, new_val );
        }
    });*/

    return item;

}

(function(){
    RC.init();
})();
                
});
