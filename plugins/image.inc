<?php
function sebd7tweaks_API_image_styles($sizes) {
  $styles = array();

  foreach ($sizes as $style => $data) {
    $data = is_array($data)? $data: array($data);
    $data = (count($data) != 2)? array($data[0], $data[0]): $data;
    $data = array('width' => $data[0], 'height' => $data[1]);
    $styles[$style] = array('effects' => array(array('name' => 'image_scale_and_crop', 'data' => $data, 'weight' => 0)));
  }

  return $styles;
}

