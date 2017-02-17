(function($) {
  Drupal.behaviors.sebd7tweaksFixTinyMCEfullscreen = {
    attach: function(context, settings) {
      var admin_menu = $('#admin-menu', Drupal.overlayChild? window.parent.document: document);

      tinyMCE.onAddEditor.add(function (mgr, ed) {
        if (ed.id == 'mce_fullscreen') {
          admin_menu.hide();
        }
      });

      tinyMCE.onRemoveEditor.add(function (mgr, ed) {
        if (ed.id == 'mce_fullscreen') {
          admin_menu.show();
        }
      });
    }
  };
}) (jQuery);

