# Makefile for A-Maze-ing project

VENV = .venv
BIN = $(VENV)/bin

.PHONY: install run debug clean fclean lint lint-strict build-package

# 仮想環境を作成し、開発依存（テスト・静的解析）ツールをインストール
install: $(VENV)
	$(BIN)/pip install flake8 mypy build

$(VENV):
	python3 -m venv $(VENV)

# 仮想環境下でプログラムを実行
run: $(VENV)
	$(BIN)/python3 a_maze_ing.py config.txt

# pdbデバッガを利用してデバッグ起動
debug: $(VENV)
	$(BIN)/python3 -m pdb a_maze_ing.py config.txt

# 不要な一時データやビルドキャッシュを安全にクリーニング
clean:
	rm -rf __pycache__
	rm -rf src/mazegen/__pycache__
	rm -rf .mypy_cache
	rm -rf .pytest_cache
	rm -rf build dist *.egg-info
	find . -type d -name "__pycache__" -exec rm -rf {} +
	find . -type f -name "*.pyc" -delete

# 仮想環境を含む全生成ファイルを完全リセット
fclean: clean
	rm -rf $(VENV)
	rm -f *.whl *.tar.gz

# 42コーディング規約に合致する厳密な静的解析とコードクオリティテスト
lint: $(VENV)
	$(BIN)/flake8 a_maze_ing.py src/
	$(BIN)/mypy --warn-return-any --warn-unused-ignores --ignore-missing-imports --disallow-untyped-defs --check-untyped-defs a_maze_ing.py src/

# 最も厳格なmy-strict検証規則の適用
lint-strict: $(VENV)
	$(BIN)/flake8 a_maze_ing.py src/
	$(BIN)/mypy --strict a_maze_ing.py src/

# 評価用のライブパッケージ再ビルドルール。成果物をプロジェクトルートへ自動展開します。
build-package: $(VENV)
	$(BIN)/pip install --upgrade build
	$(BIN)/python3 -m build
	cp dist/*.whl .
	cp dist/*.tar.gz .
	@echo "=========================================================="
	@echo "🎉 パッケージビルドに成功し、ルートディレクトリへの展開が完了しました！"
	@echo "=========================================================="