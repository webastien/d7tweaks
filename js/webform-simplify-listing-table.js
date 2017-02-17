(function($) {
  Drupal.behaviors.sebd7tweaksWebformSimplifyListingTable = {
    attach: function(context, settings) {
      var form = $('form#webform-components-form', context), index = 1, found = false;
      $('#webform-components th', form).each(function() {
        if (!found) { if ($(this).text() == Drupal.t('Value')) { found = true; } else { index++; } }
      });
      if (found) { $('table tr td:nth-child('+ index +'), table tr th:nth-child('+ index +')', form).remove(); }
    }
  };
}) (jQuery);

