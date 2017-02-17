<?php
function sebd7tweaks_API_devel_declare_features() {
  $features = array();
  // As devel tabs can "break" the design when checking the site as an admin...
  foreach (array('node', 'comment', 'user', 'taxonomy') as $entity) {
    $features["hide_tab_$entity"]
      = t('Hide devel tab for %entity entities - The tab page can always be accessed by its path', array('%entity' => $entity));
  }

  return $features;
}

