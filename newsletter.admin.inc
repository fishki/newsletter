<?php
// $Id:$
function newsletter_list_form() {
    $form['search'] = array(
        '#type' => 'fieldset',
        '#title' => t('Search'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#theme' => 'newsletter_list_search_form',
    );
    
    $title = isset($_GET['title']) ? trim($_GET['title']) : FALSE;
    $form['search']['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => filter_xss($title, array()),
        '#size' => 60,
    );
    
    $date_from = isset($_GET['date_from']) ? trim($_GET['date_from']) : FALSE;
    $form['search']['date_from'] = array(
        '#id' => 'search_date_from',
        '#type' => 'textfield',
        '#title' => t('Date from'),
        '#default_value' => filter_xss($date_from, array()),
        '#size' => 20,
    );

    $date_to = isset($_GET['date_to']) ? trim($_GET['date_to']) : FALSE;
    $form['search']['date_to'] = array(
        '#id' => 'search_date_to',
        '#type' => 'textfield',
        '#title' => t('Date to'),
        '#default_value' => filter_xss($date_to, array()),
        '#size' => 20,
    );
    $form['search']['search_button'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
    );

    $condition = array();
    $condition['where'][] = "n.type = 'newsletter'";
    $condition['args'][] = array();

    if($title) {
        $condition['where'][] = "n.title LIKE :title";
        $condition['args'][':title'] = $title;
    }

    if($date_from) {
        $condition['where'][] = "nl.sending_date >= :date_min";
        $condition['args'][':date_min'] = strtotime($date_from . ' 00:00:00');
    }

    if($date_to) {
        $condition['where'][] = "nl.sending_date <= :date_max";
        $condition['args'][':date_max'] = strtotime($date_to . ' 23:59:59');
    }
    
    //$rows_per_page = 20;
    $where_conditions = implode(' AND ', $condition['where']);
    
    $sql = "SELECT
        n.nid,
        n.title,
        nl.sending_date
        FROM {node} n
        LEFT JOIN {newsletter_list} nl ON nl.nid = n.nid
        WHERE " . $where_conditions . " ORDER BY nid DESC";
    //pre($sql); die();
    //$query_count = "SELECT COUNT(*) FROM {node} n LEFT JOIN {newsletter_list} nl ON nl.nid = n.nid WHERE " . $where_conditions;
    $query = db_select('node', 'n')->extend('PagerDefault');
    $query->leftJoin('newsletter_list', 'nl', 'n.nid = nl.nid');
    $query->fields('n', array('nid', 'title'));
    $query->fields('nl', array('sending_date'));
    $query->condition('type', 'newsletter');
    $result = $query->execute();
      pre($result); die();
      foreach($result as $row) {
        $output[$i]['nid'] = $row->nid;
        $output[$i]['title'] = $row->title;
        $i++;
      }
    
     //pre($output); die();
      //$query -> fetchAssoc();
      
      
    //$result = db_query($sql, $condition['args']);
 /*   
    while($data = db_fetch_object($result)) {
        $form['newsletter_title'][$data->nid] = array('#value' => l($data->title, 'newsletter/manage/edit/' . $data->nid));
        $form['newsletter_sending_date'][$data->nid] = array('#value' => (int)$data->sending_date ?  date('Y-m-d', $data->sending_date) : '');
        $form['actions_preview'][$data->nid] = array('#value' => l(t('Preview'), 'newsletter/template/' . $data->nid, array('attributes' => array('target' => '_blank'))));
        $form['actions_send'][$data->nid] = array('#value' => l(t('Send'), 'send_mailiing/' . $data->nid, array('attributes' => array('calss' => 'send_link'))));
    }
  */
    return $form;
  
 
}