<?php
function sebd7tweaks_API_overlay_declare_features() {
  return array(
    'lock_choice'  => t('Lock the -per user- overlay settings'),
    'disable_user' => t('Disable overlay for user creation / modification (so, it will use the front design)'),
  );
}

