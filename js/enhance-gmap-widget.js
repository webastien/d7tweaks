(function($) {
  Drupal.behaviors.sebd7tweaksGmapWidget = {
    attach: function(context, settings) {
      for (var gmap_id in settings.sebd7tweaks.gmaps) {
        $('#'+ gmap_id, context).once('sebd7tweaks-gmap', function() {
          var widget = $(this),
              addr   = $('input[name$="[name]"]', widget),
              zoom   = $('input[name$="[zoom]"]', widget),
              lat    = $('input[name$="[lat]"]',  widget),
              lon    = $('input[name$="[lon]"]',  widget);

          var get_addr   = function() { return addr.val(); };
          var get_center = function() { return new google.maps.LatLng(lat.val(), lon.val()); };
          var get_zoom   = function() { var v = parseInt(zoom.val()); return (v == null || v == '')? 14: v; }

          var gmap = new google.maps.Map($('.google-map-field-preview', widget)[0], {
            zoom:              get_zoom(),
            center:            get_center(),
            streetViewControl: false,
            draggable:         false,
            scrollwheel:       false, disableDoubleClickZoom: true,
            mapTypeId:         google.maps.MapTypeId.ROADMAP
          });

          google.maps.event.addListener(gmap, 'zoom_changed', function() { zoom.val(gmap.getZoom()); });

          var marker = new google.maps.Marker({ position: get_center(), draggable: false, map: gmap });

          var update_map = function(e) { e.preventDefault();
            var geocoder = new google.maps.Geocoder();

            geocoder.geocode({ 'address': get_addr() }, function (result, status) {
              if (status == 'OK') {
                lat.val(result[0].geometry.location.lat());
                lon.val(result[0].geometry.location.lng());

                var center = get_center();

                gmap.panTo(center);
                marker.setPosition(center);
              }
              else {
                alert(Drupal.t('Could not find location.'));
              }
            });
          };

          var button = $('<button />').text(Drupal.t('Update')).click(update_map);
          var item = addr.closest('.form-item').append(button);

          widget.before(item);
        });
      }
    }
  };
}) (jQuery);

