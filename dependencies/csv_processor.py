import csv
import os
import re
import sys
from typing import Any, List, Optional, Tuple

from .load_env import EnvironmentLoaderFactory, EnvironmentType


class SingletonMeta(type):
    """
    A Singleton.
    """
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        return cls._instances[cls]


class EnvLoader(metaclass=SingletonMeta):

    def __init__(self):
        self.env_loader = EnvironmentLoaderFactory().get_environment_loader(
            EnvironmentType.LOCAL)

    def load(self, key: str, default: Optional[str] = None) -> Any:
        return self.env_loader.load_env(key, default)


class CsvSettings:
    CSV_OUTPUT_PATH: str = EnvLoader().load(
        'CSV_OUTPUT_PATH', 'output/')
    CSV_OUTPUT_NAME: str = EnvLoader().load(
        'CSV_OUTPUT_NAME', 'processed_data')
    EXTENSION = '.csv'


def parse_filters(row_filters: str) -> List[Tuple[str, str, str]]:
    filters = []
    if row_filters:
        for line in row_filters.split('\n'):
            match = re.match(r'(\w+)([<>=!]+)(.*)', line)
            if match:
                header, operator, value = match.groups()
                filters.append((header, operator, value))
            else:
                sys.stderr.write(f"Invalid filter: '{line}'\n")
                sys.exit(1)
    return filters


def apply_filters(row: List[str], headers: List[str],
                  filters: List[Tuple[str, str, str]]) -> bool:
    for header, operator, value in filters:
        idx = headers.index(header)
        cell_value = row[idx]

        if operator == '>':
            if not cell_value > value:
                return False
        elif operator == '<':
            if not cell_value < value:
                return False
        elif operator == '=':
            if not cell_value == value:
                return False
        elif operator == '!=':
            if not cell_value != value:
                return False
        elif operator == '>=':
            if not cell_value >= value:
                return False
        elif operator == '<=':
            if not cell_value <= value:
                return False
        else:
            sys.stderr.write(
                f"Invalid operator '{operator}' in filter for header '{header}'\n"
            )
            sys.exit(1)
    return True


def process_csv_data(csv_data: str, selected_columns: str,
                     row_filters: str) -> str:
    reader = csv.reader(csv_data.splitlines())
    headers = next(reader)
    rows = list(reader)

    filters = parse_filters(row_filters)

    selected_columns_list = selected_columns.split(
        ',') if selected_columns else headers

    for col in selected_columns_list:
        if col not in headers:
            sys.stderr.write(f"Header '{col}' not found in CSV headers\n")
            sys.exit(1)

    selected_indices = [headers.index(col) for col in selected_columns_list]
    selected_indices.sort()
    sorted_columns = [headers[i] for i in selected_indices]

    filtered_rows = [
        row for row in rows if apply_filters(row, headers, filters)
    ]
    selected_rows = [[row[i] for i in selected_indices]
                     for row in filtered_rows]

    result = [sorted_columns]
    result.extend(selected_rows)

    result = '\n'.join([','.join(row) for row in result])
    save_csv_file(
        result,
        f'{CsvSettings.CSV_OUTPUT_PATH}{CsvSettings.CSV_OUTPUT_NAME}{CsvSettings.EXTENSION}'
    )
    return result


def process_csv_file(csv_file_path: str, selected_columns: str,
                     row_filters: str) -> str:
    try:
        with open(csv_file_path, 'r') as file:
            csv_data = file.read()
        return process_csv_data(csv_data, selected_columns, row_filters)
    except Exception as e:
        return str(e)


def save_csv_file(data: str, output_file: str) -> None:
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, 'w+') as file:
        file.write(data)


if __name__ == '__main__':
    #1
    csv_data = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9"
    selected_columns = "header3,header1"
    row_filters = "header1>1\nheader3<9"
    result = process_csv_data(csv_data, selected_columns, row_filters)
    #save_csv_file(result, f'{CsvSettings.CSV_OUTPUT_PATH}{CsvSettings.CSV_OUTPUT_NAME}' )
    print(result, '\n\n')

    #2
    csv_data = "col1,col2,col3,col4,col5,col6,col7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl2c1,l2c2,l2c3,l2c4,l2c5,l2c6,l2c7\nl3c1,l3c2,l3c3,l3c4,l3c5,l3c6,l3c7"
    selected_columns = "col1,col3,col4,col7"
    row_filters = 'col1>l1c1\ncol3!=l1c3'
    result = process_csv_data(csv_data, selected_columns, row_filters)
    print(result, '\n\n')

    #3
    csv_path = './data.csv'
    selected_columns = "col1,col3,col4,col7"
    row_filters = 'col1>l1c1\ncol3>l2c3'
    result = process_csv_file(csv_path, selected_columns, row_filters)
    #save_csv_file(result, 'output/processed_data.csv')
    print(result, '\n\n')

    #caso 4
    csv_data = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9"
    selected_columns = "header5,header7"
    row_filters = "header1>1\nheader3<9"
    result = process_csv_data(csv_data, selected_columns, row_filters)
    print(result, '\n\n')

    #caso 5
    csv_data = "col1,col2,col3,col4,col5,col6,col7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl2c1,l2c2,l2c3,l2c4,l2c5,l2c6,l2c7\nl3c1,l3c2,l3c3,l3c4,l3c5,l3c6,l3c7"
    selected_columns = "col1,col3,col4,col7"
    row_filters = 'col1>l1c1\ncol3#l1c3'
    result = process_csv_data(csv_data, selected_columns, row_filters)
    print(result, '\n\n')
