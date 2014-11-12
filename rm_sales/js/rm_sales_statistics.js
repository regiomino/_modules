jQuery(document).ready(function ($) {
    var flotOptions = {
    xaxis: {
        mode: "time",
        timeformat: "%d-%m-%Y"
    },
    
    legend :  {
        show : true
    },
    
    series: {           
        lines: {
            show: true,
            fill: true
        },
        points: {
            show: true
        }
    },
    
    grid: {
        hoverable: true
        
    },
    tooltip: true,
    tooltipOpts: {
        content: "'%s' of %x.1 is %y.4",
       // content: "%y.4",
        shifts: {
            x: -60,
            y: 25
        }
    }
};


 
 
var flotPath = "/rm-sales-getstatistics";

/* $graphs = array(
        'profiles' => array(
            array(
                'label' => t('created profiles'),
                'data' => $flotCreatedProfiles,
            ),
            array(
                'label' => t('created customer profiles'),
                'data' => $flotCreatedCustomerProfiles,
            ),
            array(
                'label' => t('created seller profiles'),
                'data' => $flotCreatedSellerProfiles,
            ),
            array(
                'label' => t('created trader profiles'),
                'data' => $flotCreatedTraderProfiles,
            ),
        ),
    );*/

    

function initFlot() {
    
    $.ajax({
       url: flotPath,
       type: "POST",
       dataType : 'json',
 
   }).success(function(data) {
        
        for (var p in data) {
            
           $.plot($('#'+ p+''),data[p],flotOptions);
        }
     
    });
}

initFlot();
    
});