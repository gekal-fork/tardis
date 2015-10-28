<?php

/**
 * Implementation of hook_views_plugins().
 */
function tardisviews_views_plugins() {
  $plugin = array();
  $plugin['module'] = 'tardisviews';
  $plugin['style']['tardisviews_monthlist'] = array(
    'title' => t('TARDIS Views month list'),
    'help' => t('Creates a list of years and months with links to archive content.'),
    'handler' => 'tardisviews_plugin_style_monthlist',
    'theme' => 'tardisviews_view_monthlist',
    'type' => 'normal',
    'uses options' => TRUE,
  );
  return $plugin;
}
