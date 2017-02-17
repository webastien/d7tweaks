<?php
function sebd7tweaks_API_jcarousel_declare_features() {
  $comment = t('This module is depreciated, use "flexslider" instead');
  return array('fix_resize_bug' => t('Fix the bug which move items out of the view when the browser is resized.') ." [$comment]");
}

