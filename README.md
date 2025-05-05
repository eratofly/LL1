# Язык программирования, похожий на Го (названия пока нет)

_program_ → _statement_ | _e_

_statement_ → _variable_decl_ | _assignment_ | _if_assignment_ | _for_loop_ | _while_loop_ | _function_decl_ | _function_call_ | print_call | _e_

_variable_decl_ → _variable_decl_type_ _type_ **identifier** **:=** _expression_ **;**

_variable_decl_type_ → **var** | **const**

_assignment_ → **identifier** **:=** _expression_**;**

_if_assignment_ → **if** **(** _expression_ **)** **{** _statement_ **}** | **if** **(** _expression_ **)** **{** _statement_ **}** **else** **{** _statement_ **}**

_for_loop_ → **for** **(** _init_statement_ **;** _condition_ **;** _post_statement_ **)** **{** **_statement_** **}**

_while_loop_ → **while** **(** _expression_ **)** **{** _statement_ **}**

_function_decl_ → **fun** **identifier** **(** _parameters_ **)** **:** _type_ **{** _statement_ **}**

_function_call_ → **identifier** ( arguments )**;**

print_call → **print** **identifier****;** | **print** string**;**
//тут бы добавить конкатенацию, но пока нет

_parameters_ → _parameters_list_ | _parameter_ | _e_

_parameters_list_ → _parameter_ **,** _parameters_list_ | _parameter_

_parameter_ → **identifier** **:** _type_

_arguments_ → _arguments_list_ | _argument_ | _e_

_arguments_list_ → _argument_ **,** _arguments_list_ | _argument_

_argument_ → **identifier**

_type_ → **int** | **bool** | **float** | **string** | **void**

_expression_ → _comparison_expr_ | _comparison_expr_list_

_comparison_expr_list_ → **(** _comparison_expr_ **)** _comparison_expr_sign_ _comparison_expr_list_ | **(** _comparison_expr_ **)**

_comparison_expr_ → _additive_expr_ | _additive_expr_ _additive_expr_sign_ _additive_expr_

_comparison_expr_sign_ → **&&** | **||**

_additive_expr_sign_ → **==** | **!=** | **<** | **>** | **<=** | **>=**

_additive_expr_ → _multiplicative_expr_ | _multiplicative_expr_ _multiplicative_expr_sign_ _multiplicative_expr_

_multiplicative_expr_sign_ → **+** | **-** 

_multiplicative_expr_ → _primary_expr_ | _primary_expr_ _primary_expr_sign_ _primary_expr_

_primary_expr_sign_ → * | **/** | **%**

_primary_expr_ → _number_ | **identifier** | _function_call_ | _expression_ | **!** _primary_expr_

_init_statement_ → _variable_decl_ | _assignment_ | e

_condition_ → _expression_ | e

_post_statement_ → _assignment_ | e