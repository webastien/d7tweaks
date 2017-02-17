(function($) {
  Drupal.behaviors.sebd7tweaksFileFields = {
    attach: function(context, settings) {
      var default_text = Drupal.t('Upload destination');
      for (var field_id in settings.sebd7tweaks.file_fields) {
        $('#'+ field_id, context).once('sebd7tweaks', function() {
          var custom_id  = settings.sebd7tweaks.file_fields[field_id],
              file_field = $('#'+ field_id, context).hide().after('<a id="'+ custom_id +'" class="sebd7tweaks-file"></a>'),
              custom_field = file_field.siblings('#'+ custom_id).click(function(e) { e.preventDefault(); file_field.click(); });
          // Doesn't work with jQuery change(), so...
          (document.getElementById(field_id).onchange = function() {
            var empty = (this.value == '' || typeof this.value == 'undefined');
            custom_field.toggleClass('no-linked-file', empty).text(empty? default_text: this.value);
          })();
        });
      }
    }
  };
}) (jQuery);

