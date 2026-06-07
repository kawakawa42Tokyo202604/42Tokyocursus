#!/usr/bin/env python3
import abc
import typing


class ExportPlugin(typing.Protocol):

    def process_output(self, data: list[tuple[int, str]]) -> None:
        ...


class CSVExportPlugin:

    def process_output(self, data: list[tuple[int, str]]) -> None:
        print("CSV Output:")
        print(",".join(val for rank, val in data))


class JSONExportPlugin:

    def process_output(self, data: list[tuple[int, str]]) -> None:
        print("JSON Output:")
        parts = [f'"item_{rank}": "{val}"' for rank, val in data]
        print("{" + ", ".join(parts) + "}")


class DataProcessor(abc.ABC):

    def __init__(self) -> None:
        self._storage: list[tuple[int, str]] = []
        self._next_rank: int = 0
        self._total_processed: int = 0

    @abc.abstractmethod
    def validate(self, data: typing.Any) -> bool:
        pass

    @abc.abstractmethod
    def ingest(self, data: typing.Any) -> None:
        pass

    def output(self) -> tuple[int, str]:
        if not self._storage:
            raise IndexError("No data available to output.")
        return self._storage.pop(0)

    @property
    def total_processed(self) -> int:
        return self._total_processed

    def __len__(self) -> int:
        return len(self._storage)


class NumericProcessor(DataProcessor):

    def validate(self, data: typing.Any) -> bool:
        if isinstance(data, (int, float)) and not isinstance(data, bool):
            return True
        if isinstance(data, list):
            return all(
                isinstance(x, (int, float)) and not isinstance(x, bool)
                for x in data
            )
        return False

    def ingest(
        self,
        data: typing.Union[int, float, list[typing.Union[int, float]]]
    ) -> None:
        if not self.validate(data):
            raise ValueError("Improper numeric data")
        if isinstance(data, list):
            for item in data:
                self._storage.append((self._next_rank, str(item)))
                self._next_rank += 1
                self._total_processed += 1
        else:
            self._storage.append((self._next_rank, str(data)))
            self._next_rank += 1
            self._total_processed += 1


class TextProcessor(DataProcessor):

    def validate(self, data: typing.Any) -> bool:
        if isinstance(data, str):
            return True
        if isinstance(data, list):
            return all(isinstance(x, str) for x in data)
        return False

    def ingest(self, data: typing.Union[str, list[str]]) -> None:
        if not self.validate(data):
            raise ValueError("Improper text data")
        if isinstance(data, list):
            for item in data:
                self._storage.append((self._next_rank, str(item)))
                self._next_rank += 1
                self._total_processed += 1
        else:
            self._storage.append((self._next_rank, str(data)))
            self._next_rank += 1
            self._total_processed += 1


class LogProcessor(DataProcessor):

    def validate(self, data: typing.Any) -> bool:
        def is_valid_dict(d: typing.Any) -> bool:
            if not isinstance(d, dict):
                return False
            return all(
                isinstance(k, str) and isinstance(v, str)
                for k, v in d.items()
            )

        if is_valid_dict(data):
            return True
        if isinstance(data, list):
            return all(is_valid_dict(x) for x in data)
        return False

    def ingest(
        self,
        data: typing.Union[
            dict[str, str], list[dict[str, str]]
        ]
    ) -> None:
        if not self.validate(data):
            raise ValueError("Improper log data")
        if isinstance(data, list):
            for item in data:
                val_str = ": ".join(item.values())
                self._storage.append((self._next_rank, val_str))
                self._next_rank += 1
                self._total_processed += 1
        else:
            val_str = ": ".join(data.values())
            self._storage.append((self._next_rank, val_str))
            self._next_rank += 1
            self._total_processed += 1


class DataStream:

    def __init__(self) -> None:
        self._processors: list[DataProcessor] = []

    def register_processor(self, proc: DataProcessor) -> None:
        self._processors.append(proc)

    def process_stream(self, stream: list[typing.Any]) -> None:
        for element in stream:
            handled = False
            for proc in self._processors:
                if proc.validate(element):
                    proc.ingest(element)
                    handled = True
                    break
            if not handled:
                print(
                    f"DataStream error - Can't process element "
                    f"in stream: {element}"
                )

    def output_pipeline(self, nb: int, plugin: ExportPlugin) -> None:
        for proc in self._processors:
            collected_data: list[tuple[int, str]] = []
            for _ in range(nb):
                try:
                    item = proc.output()
                    collected_data.append(item)
                except IndexError:
                    break
            if collected_data:
                plugin.process_output(collected_data)

    def print_processors_stats(self) -> None:
        print()
        print("== DataStream statistics ==")
        if not self._processors:
            print("No processor found, no data")
            return
        for proc in self._processors:
            raw_name = proc.__class__.__name__
            if raw_name.endswith("Processor"):
                name = raw_name[:-9] + " Processor"
            else:
                name = raw_name
            print(
                f"{name}: total {proc.total_processed} items processed, "
                f"remaining {len(proc)} on processor"
            )


def main() -> None:
    print("=== Code Nexus - Data Pipeline ===")
    print()
    print("Initialize Data Stream...")
    stream_manager = DataStream()
    stream_manager.print_processors_stats()

    print("Registering Processors")
    num_proc = NumericProcessor()
    text_proc = TextProcessor()
    log_proc = LogProcessor()
    stream_manager.register_processor(num_proc)
    stream_manager.register_processor(text_proc)
    stream_manager.register_processor(log_proc)

    tmp: list[typing.Any] = [
        'Hello world',
        [3.14, -1, 2.71],
        [
            {
                'log_level': 'WARNING',
                'log_message': 'Telnet access! Use ssh instead'
            },
            {
                'log_level': 'INFO',
                'log_message': 'User wil is connected'
            }
        ],
        42,
        ['Hi', 'five']
    ]
    print()
    print(f"Send first batch of data on stream: {tmp}")
    stream_manager.process_stream(tmp)
    stream_manager.print_processors_stats()

    print()
    print("Send 3 processed data from each processor to a CSV plugin:")
    csv_plugin = CSVExportPlugin()
    stream_manager.output_pipeline(3, csv_plugin)
    stream_manager.print_processors_stats()

    tmp1: list[typing.Any] = [
        21,
        ['I love AI', 'LLMs are wonderful', 'Stay healthy'],
        [
            {'log_level': 'ERROR', 'log_message': '500 server crash'},
            {
                'log_level': 'NOTICE',
                'log_message': 'Certificate expires in 10 days'
            }
        ],
        [32, 42, 64, 84, 128, 168],
        'World hello'
    ]
    print()
    print(f"Send another batch of data: {tmp1}")
    stream_manager.process_stream(tmp1)
    stream_manager.print_processors_stats()

    print()
    print("Send 5 processed data from each processor to a JSON plugin:")
    json_plugin = JSONExportPlugin()
    stream_manager.output_pipeline(5, json_plugin)
    stream_manager.print_processors_stats()


if __name__ == "__main__":
    main()
