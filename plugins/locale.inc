<?php
function sebd7tweaks_API_locale_declare_features() {
  return array(
    'dynamic_parts_warning' => t('Add a warning and a custom validation to translation form when dynamic parts are found'),
    'hide_language_filter'  => t('Hide language filter if there is only one language enabled in node admin content form'),
  );
}

