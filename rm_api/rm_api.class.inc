<?php

/**
 * @file
 * Contains \TCPDF\TCPDFDrupal.
 */

/**
 * Do not create a new instance of this class manually. Use tcpdf_get_instance().
 * @see tcpdf_get_instance()
 */
class TCPDFRegiomino extends TCPDF {


  protected $drupalHeader = array(
    'html' => NULL,
    'callback' => NULL,
  );
  protected $drupalFooter = array(
    'html' => NULL,
    'callback' => NULL,
  );
  /**
   * Sets a bunch of commonly used propeties in the TCPDF object. The propeties
   *   set by this function can be safely changed after calling the method. This
   *   method also let's the developer to change the header or footer of the pdf
   *   document without making his/her own class.
   *
   * @param array $options
   *   Associative array containing basic settings.
   *     'title' => Title of the document
   *     'subject' => Subject of the document
   *     'author' => Author of the document
   *     'logo_path' => Path to a logo wich is placed to the header
   *     'keywords' => Comma separated list of keywords
   *     'header' => Array
   *        'html' => Html code of the header.
   *        'callback' => Function that generates the header. If 'html' is set, it's ignored. Note: Not working.
   *     'footer' => Array
   *        'html' => Html code of the footer.
   *        'callback' => Function that generates the footer. If 'html' is set, it's ignored. Note: Not working.
   */
  function DrupalInitialize($options) {
    $title = isset($options['title']) ? $options['title'] : variable_get('site_name', 'Drupal');
    $author = isset($options['author']) ? $options['author'] : variable_get('site_name', 'Drupal');
    $subject = isset($options['subject']) ? $options['subject'] : variable_get('site_name', 'Drupal') ;
    $keywords = isset($options['keywords']) ? $options['keywords'] : 'pdf, drupal';
    $this->drupalHeader = isset($options['header']) ? $options['header'] : $this->drupalHeader;
    $this->drupalFooter = isset($options['footer']) ? $options['footer'] : $this->drupalFooter;

    // set document information
    $this->SetCreator(PDF_CREATOR);
    $this->SetAuthor($author);
    $this->SetTitle($title);
    $this->SetSubject($subject);
    $this->SetKeywords($keywords);

    // set default header data

    $this->setFooterFont(Array(PDF_FONT_NAME_DATA, '', 6));

    // set default monospaced font
    $this->SetDefaultMonospacedFont(PDF_FONT_MONOSPACED);

    //set margins
    $this->SetMargins(26, 26, 26);
    $this->SetHeaderMargin(0);
    $this->SetFooterMargin(28);

    //set auto page breaks
    $this->SetAutoPageBreak(TRUE, 30);

    //set image scale factor
    $this->setImageScale(1);

    // set font
    $this->SetFont('helvetica', '', 8);
    $this->AddPage();
		
		
		// get the current page break margin
		$bMargin = $this->getBreakMargin();
		// get current auto-page-break mode
		$auto_page_break = $this->getAutoPageBreak();
		// disable auto-page-break
		$this->SetAutoPageBreak(false, 0);
		// set bacground image
		$img_file = '<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<svg version="1.0" id="Ebene_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
	 width="595.28px" height="841.89px" viewBox="0 0 595.28 841.89" enable-background="new 0 0 595.28 841.89" xml:space="preserve">
<path fill="#FFFFFF" stroke="#DDDDDD" stroke-miterlimit="10" d="M551.721,31.526c-2.976,0-5.354,1.493-6.754,4.009
	c-0.625-2.514-2.588-4.009-5.434-4.009c-0.866,0-1.684,0.121-2.443,0.36l-0.209-0.635l-6.348,1.245v-0.689
	c0.24-0.554,0.372-1.162,0.372-1.8c0-2.523-2.034-4.577-4.533-4.577c-2.564,0-4.648,2.053-4.648,4.577
	c0,0.84,0.23,1.626,0.625,2.303l-0.225,0.023v2.141c-1.169-2.6-3.762-2.948-5.053-2.948c-1.36,0-2.592,0.317-3.681,0.949
	c-0.895-0.562-2.058-0.949-3.414-0.949c-0.88,0-1.6,0.125-2.203,0.322l-0.188-0.603l-7.766,1.523l0.293,1.377
	c-1.27-1.576-3.181-2.62-5.901-2.62c-2.829,0-5.117,1.348-6.538,3.642v-3.723l-0.217,0.023c0.278-0.152,0.559-0.318,0.832-0.504
	c1.087-0.673,1.965-1.487,2.611-2.42c2.148-3.106,1.355-6.903,1.26-7.321l-1.171-5.12l-2.456,3.578
	c-0.571,0.138-1.292,0.352-2.068,0.676c-1.071-2.095-2.484-3.471-2.687-3.662l-0.105-0.101l-3.778-3.838v4.701
	c-5.184,5.951-3.105,10.184-1.45,13.449c-1.753,3.933-6.83-2.47-13.647,3.973c0,0-6.104-6.757-12.808-0.244
	c0.464-4.063-1.663-7.33-7.515-7.33c-0.985,0.002-9.27-0.104-9.27-0.104v23.415c0,0,9.647,0.06,17.11,0.06
	c0-0.691,0.031-1.321,0.031-2.397c4.054,4.245,9.071,2.427,11.185,1.445c0,10.061,16.607,8.219,16.869,1.158
	c2.797,0,7.273,0,7.273,0v-3.105c1.387,2.208,3.607,3.48,6.396,3.48c2.341,0,4.923-1.043,6.539-3.658v3.282h21.516h0.597h7.214
	h0.599h14.648v-3.105c1.386,2.208,3.607,3.48,6.396,3.48c3.755,0,8.139-2.683,8.139-10.242
	C559.717,36.427,557.616,31.526,551.721,31.526z"/>
<path fill-rule="evenodd" clip-rule="evenodd" fill="#96BE1F" d="M446.75,39.979c2.036,2.195,2.916,5.674,4.208,8.697
	c-1.771,0-3.539,0-5.309,0c-0.421-1.357-0.912-2.997-1.402-4.46c-0.451-1.344-0.807-3.286-2.404-3.345
	c-0.233,2.378-0.033,5.24-0.101,7.805c-1.535,0-3.071,0-4.607,0c0-6.504,0-13.008,0-19.512c-0.004-0.19,0.075-0.289,0.201-0.335
	c4.212,0.225,9.395-0.769,11.719,1.896C451.321,33.323,450.325,39.641,446.75,39.979z M444.747,36.522
	c0.575-0.675,0.6-2.386,0-3.011c-0.623-0.65-1.738-0.349-3.005-0.446c0,1.338,0,2.676,0,4.014
	C442.885,36.965,444.041,37.352,444.747,36.522z"/>
<path fill-rule="evenodd" clip-rule="evenodd" fill="#96BE1F" d="M464.036,42.81c-2.438,0-4.874,0-7.313,0
	c-0.345,3.788,4.296,2.809,5.509,1.227c0.63,0.898,1.241,1.815,1.704,2.899c-1.724,1.49-3.761,2.309-6.411,2.007
	c-3.063-0.349-5.147-2.971-5.409-6.467c-0.197-2.632,0.239-5.699,2.104-7.47c2.245-2.133,6.761-1.865,8.515,0.669
	C463.979,37.472,464.046,39.84,464.036,42.81z M456.724,39.576c0.969,0,1.937,0,2.905,0
	C460.266,36.218,456.144,36.391,456.724,39.576z"/>
<path fill-rule="evenodd" clip-rule="evenodd" fill="#96BE1F" d="M479.452,36.873c-0.358,0.456-1.003,0.593-1.803,0.557
	c0.274,0.662,0.663,1.923,0.301,3.122c-0.667,2.2-2.99,2.973-5.61,3.233c-0.362,0.037-1.062-0.265-1.302,0.335
	c0.249,0.79,1.128,0.54,1.603,0.557c1.89,0.071,3.604-0.04,4.809,0.892c2.05,1.584,1.677,5.886-0.302,7.582
	c-3.11,2.668-13.041,2.642-11.317-4.237c1.302,0,2.604,0,3.906,0c0.299,0.143,0.084,0.511,0.1,0.668
	c0.024,0.236,0.039,0.502,0.201,0.781c0.788,1.354,4.47,0.717,4.407-1.003c-0.07-1.93-5.209-0.783-6.711-1.449
	c-1.904-0.846-2.131-4.039,0.101-5.241c-2.006-0.83-2.379-4.932-0.902-6.801c1.191-1.508,3.49-2.099,5.51-2.007
	c1.908,0.087,4.329,1.512,5.708-0.334C478.735,34.474,478.949,35.836,479.452,36.873z M471.84,40.552
	c0.285,0.022,0.954-0.005,1.302-0.335c0.707-0.667,0.428-2.397-0.2-2.787c-0.243-0.151-0.642-0.137-1.002-0.111
	C469.946,37.461,470.029,40.411,471.84,40.552z"/>
<polygon fill="#96BE1F" points="481.352,48.675 481.352,34.129 485.732,33.671 485.732,48.675 "/>
<path fill="#96BE1F" d="M494.152,49.047c-3.694,0-5.985-2.806-5.985-7.703c0-4.781,2.349-7.817,6.128-7.817
	c3.293,0,5.984,2.09,5.984,7.388C500.279,46.757,497.216,49.047,494.152,49.047 M494.209,36.907c-0.888,0-1.346,0.859-1.346,4.524
	c0,2.862,0.145,4.237,1.375,4.237c1.03,0,1.346-1.375,1.346-4.896C495.584,37.822,495.24,36.907,494.209,36.907"/>
<path fill="#BD1729" d="M516.459,48.675v-9.478c0-1.603-0.314-1.861-1.088-1.861c-0.344,0-0.859,0.143-1.432,0.745v10.594h-4.324
	v-9.45c0-1.545-0.171-1.889-1.116-1.889c-0.372,0-0.83,0.143-1.403,0.745v10.594h-4.381V38.339c0-1.489-0.086-2.548-0.4-4.009
	l3.979-0.772c0.143,0.458,0.229,0.887,0.286,1.432c0.83-0.716,1.574-1.461,3.493-1.461c1.661,0,2.893,0.83,3.293,1.546
	c0.717-0.716,1.891-1.546,3.809-1.546c3.007,0,3.638,2.033,3.638,4.066v11.081H516.459z"/>
<path fill="#BD1729" d="M526.396,32.525c-1.374,0-2.549-1.145-2.549-2.519c0-1.375,1.204-2.52,2.636-2.52
	c1.402,0,2.52,1.145,2.52,2.52C529.003,31.38,527.829,32.525,526.396,32.525 M524.249,48.675V34.129l4.382-0.458v15.004H524.249z"/>
<path fill="#BD1729" d="M538.938,48.675v-9.822c0-1.604-0.485-1.546-1.087-1.546c-0.344,0-0.831,0.2-1.403,0.801v10.566h-4.381
	V38.339c0-1.489-0.087-2.548-0.4-4.009l3.979-0.772c0.114,0.343,0.259,0.83,0.286,1.288c0.83-0.716,2.005-1.317,3.723-1.317
	c2.806,0,3.637,1.775,3.637,3.78v11.368H538.938z"/>
<path fill="#BD1729" d="M551.712,49.047c-3.694,0-5.984-2.806-5.984-7.703c0-4.781,2.349-7.817,6.127-7.817
	c3.293,0,5.984,2.09,5.984,7.388C557.839,46.757,554.775,49.047,551.712,49.047 M551.77,36.907c-0.888,0-1.346,0.859-1.346,4.524
	c0,2.862,0.143,4.237,1.374,4.237c1.03,0,1.345-1.375,1.345-4.896C553.143,37.822,552.799,36.907,551.77,36.907"/>
<path fill="#96BE1F" d="M482.896,31.593c0.191-0.211,0.425-0.483,0.676-0.807c0.847-0.187,2.363-0.623,3.706-1.531
	c0.868-0.528,1.558-1.153,2.049-1.856c1.673-2.395,0.978-5.516,0.947-5.647l-0.146-0.63l-0.319,0.46
	c-0.562,0.094-2.586,0.501-4.306,1.703c-0.761-2.85-2.937-4.913-3.034-5.004l-0.13-0.123l-0.356-0.359v0.572
	c-0.549,0.536-3.251,3.354-3.271,6.804c-0.02,3.455,2.659,6.301,3.196,6.838l0.004,0.537l0.48-0.443
	c0.014-0.012,0.2-0.184,0.479-0.489"/>
<g>
	<defs>
		<path id="SVGID_1_" d="M480.412,22.116c-0.602,0.717-0.475,1.95,0.283,2.751c0.762,0.8,1.865,0.868,2.468,0.152
			c0.602-0.718,0.474-1.952-0.285-2.754c-0.406-0.429-0.913-0.647-1.387-0.647C481.079,21.619,480.691,21.783,480.412,22.116"/>
	</defs>
	<clipPath id="SVGID_2_">
		<use xlink:href="#SVGID_1_"  overflow="visible"/>
	</clipPath>
	
		<radialGradient id="SVGID_3_" cx="-566.2637" cy="997.9844" r="0.437" gradientTransform="matrix(-5.1152 6.101 -6.4564 -6.8118 4029.0078 10278.2617)" gradientUnits="userSpaceOnUse">
		<stop  offset="0" style="stop-color:#FFFFFF"/>
		<stop  offset="0.022" style="stop-color:#FFFFFF"/>
		<stop  offset="0.4505" style="stop-color:#8FBE44"/>
		<stop  offset="1" style="stop-color:#497C4F"/>
	</radialGradient>
	<polygon clip-path="url(#SVGID_2_)" fill="url(#SVGID_3_)" points="481.666,19.405 477.979,23.803 481.908,27.949 485.596,23.551 	
		"/>
</g>
<path fill="#96BE1F" d="M38.934,820.381c0.834-0.923,1.851-2.105,2.944-3.518c3.688-0.813,10.293-2.71,16.142-6.666
	c3.781-2.301,6.784-5.023,8.924-8.086c7.286-10.434,4.258-24.027,4.126-24.598l-0.634-2.743l-1.391,2.003
	c-2.446,0.413-11.263,2.187-18.754,7.418c-3.313-12.411-12.79-21.399-13.215-21.795l-0.566-0.535l-1.552-1.562v2.492
	c-2.391,2.335-14.16,14.611-14.245,29.635c-0.085,15.049,11.582,27.448,13.921,29.783l0.017,2.339l2.093-1.931
	c0.06-0.052,0.872-0.8,2.084-2.127"/>
<g>
	<defs>
		<path id="SVGID_4_" d="M28.118,779.101c-2.62,3.126-2.067,8.494,1.233,11.981c3.318,3.488,8.125,3.781,10.749,0.664
			c2.62-3.131,2.063-8.503-1.242-11.995c-1.77-1.867-3.977-2.815-6.04-2.815C31.023,776.936,29.334,777.65,28.118,779.101"/>
	</defs>
	<clipPath id="SVGID_5_">
		<use xlink:href="#SVGID_4_"  overflow="visible"/>
	</clipPath>
	
		<radialGradient id="SVGID_6_" cx="-458.498" cy="981.752" r="1.9023" gradientTransform="matrix(-5.1152 6.101 -6.4564 -6.8118 4029.0078 10278.2617)" gradientUnits="userSpaceOnUse">
		<stop  offset="0" style="stop-color:#FFFFFF"/>
		<stop  offset="0.022" style="stop-color:#FFFFFF"/>
		<stop  offset="0.4505" style="stop-color:#8FBE44"/>
		<stop  offset="1" style="stop-color:#497C4F"/>
	</radialGradient>
	<polygon clip-path="url(#SVGID_5_)" fill="url(#SVGID_6_)" points="33.579,767.288 17.522,786.446 34.634,804.507 50.695,785.349 	
		"/>
</g>
<rect y="824.549" fill="#96BE1F" width="595.638" height="17.855"/>
</svg>';
		$this->ImageSVG('@' . $img_file, $x=0, $y=0, $w='', $h='', $link='', $align='', $palign='', $border=0, $fitonpage=false);
		// restore auto-page-break status
		$this->SetAutoPageBreak($auto_page_break, $bMargin);
		// set the starting point for the page content
		$this->setPageMark();

  }

  /**
   * Sets the header of the document.
   * @return NULL
   */
  public function Header() {
    if (!$this->DrupalGenRunningSection($this->drupalHeader)) {
      return parent::Header();
    }
  }

  /**
   * Sets the footer of the document.
   * @return NULL
   */
  public function Footer() {
    if (!$this->DrupalGenRunningSection($this->drupalFooter)) {
      return parent::Footer();
    }
  }

  /**
   * Generates a header or footer for the pdf document.
   *
   * @param array $container
   * @see DrupalInitialize()
   *
   * @return FALSE if the container did not store any useful information to generate
   *   the document.
   */
  private function DrupalGenRunningSection($container) {
    if (!empty($container['html'])) {
      $this->writeHTML($container['html']);
      return TRUE;
    }
    elseif (!empty($container['callback']) && function_exists($container['callback'])) {
      $that = &$this;
      call_user_func($container['callback'], $that);
      return TRUE;
    }
    return FALSE;
  }
}
