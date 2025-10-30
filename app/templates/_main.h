#include "base.h"
#include "style.h"
#include "home.h"
#include "ant_config.h"

String style()
{
    String content;
    content += FPSTR(style_css);
    return content;
}

String index_page()
{
    String content;
    content += FPSTR(base_head_html);
    content += FPSTR(header);
    content += FPSTR(home_content);
    content += FPSTR(base_footer_html);
    return content;
}

String ant_config_page()
{
    String content;
    content += FPSTR(base_head_html);
    content += FPSTR(header);
    content += FPSTR(ant_config_html);
    content += FPSTR(ant_config_script);
    content += FPSTR(base_footer_html);
    return content;
}

String one_ant_config_page()
{
    String content;
    content += FPSTR(base_head_html);
    content += FPSTR(header);
    content += FPSTR(one_ant_config_html);
    content += FPSTR(one_ant_config_script);
    content += FPSTR(base_footer_html);
    return content;
}