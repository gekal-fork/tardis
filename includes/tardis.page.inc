<?php

/**
 * This function builds the page.
 *
 * @return array
 *   A rendered, paged list of node teasers. 
 */
function tardis_page() {
  // How did the user choose to append the current date to the page title? 
  $tardis_page_title_date = variable_get('tardis_page_title_date', 'no_date');

  // Unless the user asked not to append the date to the page title: 
  if ($tardis_page_title_date != 'no_date') {
    // Get the current page title. 
    $title = drupal_get_title();

    // Retrieve URL arguments. 
    $tardis_query_arg = _tardis_query_arg();

    // If we have a month, there's a distinction between
    // how months are displayed: numbers or names. 
    if (count($tardis_query_arg) > 1) {
      switch ($tardis_page_title_date) {
        // Month names:
        case 'years_month_names':
          $tardis_month_names = _tardis_month_names(); // Retrieve month names using this helper function. 
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
    if (count($tardis_query_arg) < 2) {
      drupal_set_title($title . ": $tardis_query_arg[0]");
    }
  }

  // Calls the page query function 
  $result = _tardis_query_page();

  // Assemble a list of NIDs 
  $nids = array();
  foreach ($result as $data) {
    $nids[] = $data->nid;
  }

  // Load a list of node objects with the NIDs 
  $nodes = node_load_multiple($nids);
  $views = node_view_multiple($nodes, 'teaser'); // Future Display Suite integration right here 

  // Add those objects to the output and render them as teasers 
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
function _tardis_query_page() {
  // This is the query. First it sets how many nodes per page: 
  $tardis_page_nodes_per_page = variable_get('tardis_page_nodes_per_page', 10);

  // Then it takes the node types that were chosen to be displayed: 
  $tardis_page_node_types = variable_get('tardis_page_node_types', array('page'));

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
    ->condition('type', $tardis_page_node_types)
    ->limit($tardis_page_nodes_per_page)
    ->orderBy('created', 'DESC');

  // Were a year AND a month supplied? Let's see the last two arguments. 
  // This helper function returns the year and month from the URL, if present. 
  $tardis_query_arg = _tardis_query_arg();

  // This variable is 0 by default; if any date modifies our query, 
  // it turns to 1. 
  $tardis_query_hasdate = 0;

  // Is there a year and a month in the URL? 
  if (count($tardis_query_arg) > 1) {
    $tardis_query_year = $tardis_query_arg[0];
    $tardis_query_month = $tardis_query_arg[1];

    // Define the first day of a month and the next as limits. 
    $tardis_query_date_min = $tardis_query_year . '-' . $tardis_query_month . '-01';
    $tardis_query_month++;
    $tardis_query_date_max = $tardis_query_year . '-' . $tardis_query_month . '-01';

    $tardis_query_hasdate = 1;
  }

  // Or just a year? 
  if (count($tardis_query_arg) < 2) {
    $tardis_query_year = $tardis_query_arg[0];

    // Define the first day of the year and the next as limits. 
    $tardis_query_date_min = $tardis_query_year . '-01-01';
    $tardis_query_year++;
    $tardis_query_date_max = $tardis_query_year . '-01-01';

    $tardis_query_hasdate = 1;
  }

  // So if the URL supplies one or two valid arguments, let's use them 
  // to filter only nodes from a given year and month (if applicable). 
  $tardis_query_hasdate == 1 ? $query->condition('created', array(strtotime($tardis_query_date_min), strtotime($tardis_query_date_max)), 'BETWEEN') : "";

  return $query->execute()->fetchAllAssoc('nid');
}