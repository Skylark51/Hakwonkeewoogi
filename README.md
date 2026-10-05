# 학원키우기 (Hakwonkeewoogi)

풋볼매니저/야구9단 계열의 데이터 중심 학원 경영 시뮬레이션 프로토타입입니다.

## 현재 프로토타입

- C++20 + Qt6 Widgets
- 주 단위 시간 진행
- 학생 성적/스트레스/만족도 변화
- 강사 능력치
- 학원 재정(수강료, 급여, 임대료)
- 평판 및 평균 성적
- 학생/강사/재정 화면

## 빌드

### 요구사항
- CMake 3.24+
- C++20 compiler
- Qt 6.x (Widgets)

### Windows
```bash
cmake -S . -B build
cmake --build build --config Release
```

실행 파일은 일반적으로 `build/Release/HakwonKeewoogi.exe` 에 생성됩니다.

## GitHub에서 실행할 수 있나?

네이티브 C++/Qt 실행 파일은 GitHub Pages에서 직접 실행되지 않습니다.

현재 저장소는 GitHub Actions에서 Windows 실행 파일을 자동 빌드할 수 있도록 구성합니다.
브라우저에서 바로 실행하려면 이후 별도의 WebAssembly(Emscripten/Qt for WebAssembly) 타깃을 추가할 수 있습니다.

## 초기 게임 방향

핵심 루프:

```text
학생 모집 → 반 편성/강사 배치 → 수업 → 시험/성적 변화
        ↓                         ↑
     재정/평판 ← 학부모 만족도 ← 학생 성장
```

장기적으로 다음 시스템을 확장하는 것을 전제로 합니다.

- 지역/상권
- 과목별 반 편성
- 강사 채용 시장
- 학생 잠재력 및 성향
- 학교/대학 입시
- 경쟁 학원 AI
- 광고/시설/브랜드
- 랜덤 이벤트
- SQLite 세이브
