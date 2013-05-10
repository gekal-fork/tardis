<?php

/**
 * @file
 * Page include file that renders the TARDIS list of recent nodes.
 */

/**
 * This function builds each page according to the argument received.
 *
 * @return array
 *   A rendered, paged list of node teasers.
 */
function tardis_page($tardis_page_name) {
  // $tardis_page_name is the argument used to retrieve a TARDIS page from
  // the db. It's immediately stripped from the $args array afterwards.
  $args = func_get_args();
  array_shift($args);
  $tardis_page = db_select('tardis_pages', 'tp')
    ->fields('tp')
    ->condition('name', $tardis_page_name)
    ->execute()
    ->fetchAssoc('name');

  // Those are the page settings as retrieved from the db:
  $name = $tardis_page['name'];
  $title_date = $tardis_page['title_date'];
  $nodes_per_page = $tardis_page['nodes_per_page'];
  // Node types are stored in serialized form and must be unserialized.
  $node_types = unserialize($tardis_page['node_types']);

/**/
        // If TARDIS Date module is on, fetch the date field used in the query.
        if (module_exists('tardisdate')) {
          $tardisdate_field = ($tardis_page['tardisdate_field'] != '') ? $tardis_page['tardisdate_field'] : 'created';
        }
        else {
          $tardisdate_field = '';
        }
/**/

/**/
        // If TARDIS Internationalization is on, fetch the query language.
        // There might be no language yet, so we need a default value.
        // Also the language is an array, which is stored in serialized form.
        if (module_exists('tardisi18n')) {
          $tardisi18n_lang = ($tardis_page['tardisi18n_lang'] != '') ? unserialize($tardis_page['tardisi18n_lang']) : array('cur_lang' => 'cur_lang');
        }
        else {
          $tardisi18n_lang = '';
        }
/**/

  // How did the user choose to append the current date to the page title?
  $tardis_page_title_date = $title_date;

  // Unless the user asked not to append the date to the page title:
  if ($tardis_page_title_date != 'no_date') {
    // Get the current page title.
    $title = drupal_get_title();

    // Retrieve URL arguments.
    $tardis_query_arg = _tardis_query_arg($nodes_per_page, $node_types);

    // If we have a month, there's a distinction between
    // how months are displayed: numbers or names.
    if (count($tardis_query_arg) > 1) {
      switch ($tardis_page_title_date) {
        // Month names:
        case 'years_month_names':
          // Retrieve month names using this helper function.
          $tardis_month_names = _tardis_month_names();
          $tardis_month_number = $tardis_query_arg[1];
          drupal_set_title($title . ": $tardis_month_names[$tardis_month_number], $tardis_query_arg[0]");
          break;

        // Month numbers:
        case 'years_month_numbers':
          drupal_set_title($title . ": $tardis_query_arg[1]/$tardis_query_arg[0]");
          break;
      }
    }

    // If there's only a year, that distinction is not needed.
    if (count($tardis_query_arg) == 1 && $tardis_query_arg[0] != '') {
      drupal_set_title($title . ": $tardis_query_arg[0]");
    }
  }

  // Calls the page query function.
  $result = _tardis_query_page($nodes_per_page, $node_types, $tardisdate_field, $tardisi18n_lang);

  // Assemble a list of NIDs.
  $nids = array();
  foreach ($result as $data) {
    $nids[] = $data->nid;
  }

  // Load a list of node objects with the NIDs.
  $nodes = node_load_multiple($nids);

  // Future Display Suite integration right here:
  $views = node_view_multiple($nodes, 'teaser');

  // Add those objects to the output and render them as teasers.
  $output = '';
  foreach ($views as $view) {
    $output .= render($view);
  }

  // Page results?
  $output .= theme('pager');

  return $output;
}

/**
 * This function queries the db to build a page.
 *
 * @return array
 *   An array of nodes ready for rendering.
 */
function _tardis_query_page($nodes_per_page, $node_types, $tardisdate_field, $tardisi18n_lang) {
  // This is the query. The variable $nodes_per_page sets how many nodes
  // per page to be displayed.
  // The variable $node_types aids in filtering the node types that were chosen
  // to be displayed.
  // The query also limits nodes by access permission,
  // so if users have no permissions for certain node types,
  // they won't see them.
  $query = db_select('node', 'n')
    ->fields('n', array(
      'nid',
      'type',
      'created',
    ))
    ->addTag('node_access')
    ->extend('PagerDefault')
    ->condition('type', $node_types)
    ->limit($nodes_per_page);

/**/
  // TARDIS Internationalization query join.
  // Allows TARDIS Internationalization to change the query, restricting it
  // to nodes that are in a certain language.
  if (module_exists('tardisi18n')) {
    _tardisi18n_join_query($query, $tardisi18n_lang);
  }
  else {
    $tardisi18n_lang = '';
  }
/**/

/**/
  // TARDIS Date query join.
  // Allows TARDIS Date to change the query, restricting it to nodes that
  // have a certain date.module field.
  if (module_exists('tardisdate')) {
    _tardisdate_join_query($query, $tardisdate_field);

    // Check for any date.module field that might change the query.
    $tardis_query_date = $tardisdate_field;
  }
  else {
    $tardis_query_date = 'created';
  }
/**/

  if (!module_exists('tardisdate')) {
    // If TARDIS Date is disabled,  order the query by the default created date.
    $query->orderBy('created', 'DESC');
  }

  // Were a year AND a month supplied? Let's see the last two arguments.
  // This helper function returns the year and month from the URL, if present.
  $tardis_query_arg = _tardis_query_arg();

  // If there are arguments to work with:
  if (isset($tardis_query_arg[0]) && $tardis_query_arg[0] != '') {
    // Is there a year and a month in the URL?
    if (count($tardis_query_arg) > 1 && preg_match('/^[0-9]{4}$/', $tardis_query_arg[0]) && preg_match('/^[0-9]{2}$/', $tardis_query_arg[1])) {
      $tardis_query_year = $tardis_query_arg[0];
      $tardis_query_month = $tardis_query_arg[1];

      // Define the first day of a month and the next as limits.
      $tardis_query_last_day = _tardis_query_last_day($tardis_query_year, $tardis_query_month);
      $tardis_query_date_max = $tardis_query_year . '-' . $tardis_query_month . '-' . $tardis_query_last_day;
      $tardis_query_date_min = $tardis_query_year . '-' . $tardis_query_month . '-01';
    }

    // Or just a year?
    if (count($tardis_query_arg) < 2 && preg_match('/^[0-9]{4}$/', $tardis_query_arg[0])) {
      $tardis_query_year = $tardis_query_arg[0];

      // Define the first day of the year and the next as limits.
      $tardis_query_date_min = $tardis_query_year . '-01-01';
      $tardis_query_date_max = $tardis_query_year . '-12-31';
    }

/**/
    if (module_exists('tardisdate') && $tardis_query_date != 'created') {
      // date.module fields are stored as YYYY-MM-DD HH:MM:SS
      // and need no conversion.
      $tardis_query_date .= '_value';
    }
    else {
      // Base table Created fields are stored as timestamps
      // and need to be converted.
      $tardis_query_date_min = strtotime($tardis_query_date_min);
      $tardis_query_date_max = strtotime($tardis_query_date_max);
    }

    // So if the URL supplies one or two valid arguments, let's use them
    // to filter only nodes from a given year and month (if applicable).
    $query->condition($tardis_query_date, array($tardis_query_date_min, $tardis_query_date_max), 'BETWEEN');
/**/
  }

  return $query->execute()->fetchAllAssoc('nid');
}
