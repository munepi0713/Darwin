#pragma once

namespace GarnetControlPanel {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1 の概要
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::Label^  label1;
    private: System::Windows::Forms::TextBox^  portNumberBox;









    private: System::Windows::Forms::Label^  label2;
    private: System::Windows::Forms::Label^  label3;
    private: System::Windows::Forms::Label^  jobCountH;

    private: System::Windows::Forms::Label^  label5;
    private: System::Windows::Forms::Label^  jobCountM;

    private: System::Windows::Forms::Label^  label7;
    private: System::Windows::Forms::Label^  jobCountL;

    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar1;

    private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar2;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar3;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar4;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar5;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar6;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar7;
    private: System::Windows::Forms::ProgressBar^  threadMicroJobsBar8;















    private: System::Windows::Forms::Label^  label10;
    private: System::Windows::Forms::CheckBox^  threadEnabled1;
    private: System::Windows::Forms::CheckBox^  threadEnabled2;
    private: System::Windows::Forms::CheckBox^  threadEnabled3;
    private: System::Windows::Forms::CheckBox^  threadEnabled4;
    private: System::Windows::Forms::CheckBox^  threadEnabled5;
    private: System::Windows::Forms::CheckBox^  threadEnabled6;
    private: System::Windows::Forms::CheckBox^  threadEnabled7;
    private: System::Windows::Forms::CheckBox^  threadEnabled8;
    private: System::Windows::Forms::Label^  threadMicroJobCount1;
    private: System::Windows::Forms::Label^  threadMicroJobCount2;
    private: System::Windows::Forms::Label^  threadMicroJobCount3;
    private: System::Windows::Forms::Label^  threadMicroJobCount4;
    private: System::Windows::Forms::Label^  threadMicroJobCount5;
    private: System::Windows::Forms::Label^  threadMicroJobCount6;
    private: System::Windows::Forms::Label^  threadMicroJobCount7;
    private: System::Windows::Forms::Label^  threadMicroJobCount8;
    private: System::Windows::Forms::Label^  label6;
    private: System::Windows::Forms::Label^  label8;
    private: System::Windows::Forms::Button^  imageCacheClearButton;
    private: System::Windows::Forms::Button^  button1;
    private: System::Windows::Forms::Label^  label4;


























    protected: 

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->portNumberBox = (gcnew System::Windows::Forms::TextBox());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->label3 = (gcnew System::Windows::Forms::Label());
            this->jobCountH = (gcnew System::Windows::Forms::Label());
            this->label5 = (gcnew System::Windows::Forms::Label());
            this->jobCountM = (gcnew System::Windows::Forms::Label());
            this->label7 = (gcnew System::Windows::Forms::Label());
            this->jobCountL = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobsBar1 = (gcnew System::Windows::Forms::ProgressBar());
            this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
            this->threadMicroJobsBar3 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadMicroJobsBar4 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadMicroJobsBar5 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadMicroJobsBar6 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadMicroJobsBar7 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadMicroJobsBar8 = (gcnew System::Windows::Forms::ProgressBar());
            this->threadEnabled1 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled2 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled3 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled4 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled5 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled6 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled7 = (gcnew System::Windows::Forms::CheckBox());
            this->threadEnabled8 = (gcnew System::Windows::Forms::CheckBox());
            this->threadMicroJobCount8 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount1 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount2 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount3 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount4 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount5 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount6 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobCount7 = (gcnew System::Windows::Forms::Label());
            this->threadMicroJobsBar2 = (gcnew System::Windows::Forms::ProgressBar());
            this->label10 = (gcnew System::Windows::Forms::Label());
            this->label6 = (gcnew System::Windows::Forms::Label());
            this->label8 = (gcnew System::Windows::Forms::Label());
            this->imageCacheClearButton = (gcnew System::Windows::Forms::Button());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->label4 = (gcnew System::Windows::Forms::Label());
            this->tableLayoutPanel1->SuspendLayout();
            this->SuspendLayout();
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(6, 17);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(27, 13);
            this->label1->TabIndex = 0;
            this->label1->Text = L"Port";
            this->label1->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
            // 
            // portNumberBox
            // 
            this->portNumberBox->Location = System::Drawing::Point(39, 14);
            this->portNumberBox->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->portNumberBox->Name = L"portNumberBox";
            this->portNumberBox->Size = System::Drawing::Size(59, 21);
            this->portNumberBox->TabIndex = 1;
            this->portNumberBox->Text = L"23456";
            this->portNumberBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox1_TextChanged);
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Location = System::Drawing::Point(6, 48);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(53, 13);
            this->label2->TabIndex = 10;
            this->label2->Text = L"Job Count";
            this->label2->Click += gcnew System::EventHandler(this, &Form1::label2_Click);
            // 
            // label3
            // 
            this->label3->AutoSize = true;
            this->label3->Location = System::Drawing::Point(70, 48);
            this->label3->Name = L"label3";
            this->label3->Size = System::Drawing::Size(17, 13);
            this->label3->TabIndex = 11;
            this->label3->Text = L"H:";
            this->label3->Click += gcnew System::EventHandler(this, &Form1::label3_Click);
            // 
            // jobCountH
            // 
            this->jobCountH->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->jobCountH->Location = System::Drawing::Point(93, 47);
            this->jobCountH->Name = L"jobCountH";
            this->jobCountH->Size = System::Drawing::Size(50, 15);
            this->jobCountH->TabIndex = 12;
            this->jobCountH->Text = L"0";
            this->jobCountH->TextAlign = System::Drawing::ContentAlignment::TopRight;
            this->jobCountH->Click += gcnew System::EventHandler(this, &Form1::jobCountH_Click);
            // 
            // label5
            // 
            this->label5->AutoSize = true;
            this->label5->Location = System::Drawing::Point(147, 48);
            this->label5->Name = L"label5";
            this->label5->Size = System::Drawing::Size(19, 13);
            this->label5->TabIndex = 13;
            this->label5->Text = L"M:";
            this->label5->Click += gcnew System::EventHandler(this, &Form1::label5_Click);
            // 
            // jobCountM
            // 
            this->jobCountM->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->jobCountM->Location = System::Drawing::Point(169, 47);
            this->jobCountM->Name = L"jobCountM";
            this->jobCountM->Size = System::Drawing::Size(50, 15);
            this->jobCountM->TabIndex = 14;
            this->jobCountM->Text = L"0";
            this->jobCountM->TextAlign = System::Drawing::ContentAlignment::TopRight;
            this->jobCountM->Click += gcnew System::EventHandler(this, &Form1::label6_Click);
            // 
            // label7
            // 
            this->label7->AutoSize = true;
            this->label7->Location = System::Drawing::Point(225, 48);
            this->label7->Name = L"label7";
            this->label7->Size = System::Drawing::Size(15, 13);
            this->label7->TabIndex = 15;
            this->label7->Text = L"L:";
            this->label7->Click += gcnew System::EventHandler(this, &Form1::label7_Click);
            // 
            // jobCountL
            // 
            this->jobCountL->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->jobCountL->Location = System::Drawing::Point(244, 47);
            this->jobCountL->Name = L"jobCountL";
            this->jobCountL->Size = System::Drawing::Size(50, 15);
            this->jobCountL->TabIndex = 16;
            this->jobCountL->Text = L"0";
            this->jobCountL->TextAlign = System::Drawing::ContentAlignment::TopRight;
            this->jobCountL->Click += gcnew System::EventHandler(this, &Form1::label8_Click);
            // 
            // threadMicroJobsBar1
            // 
            this->threadMicroJobsBar1->Location = System::Drawing::Point(56, 4);
            this->threadMicroJobsBar1->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar1->Name = L"threadMicroJobsBar1";
            this->threadMicroJobsBar1->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar1->TabIndex = 17;
            this->threadMicroJobsBar1->Value = 50;
            // 
            // tableLayoutPanel1
            // 
            this->tableLayoutPanel1->ColumnCount = 3;
            this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
                25.59242F)));
            this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
                74.40759F)));
            this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 
                46)));
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar3, 1, 2);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar4, 1, 3);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar5, 1, 4);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar6, 1, 5);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar7, 1, 6);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar8, 1, 7);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled1, 0, 0);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled2, 0, 1);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled3, 0, 2);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled4, 0, 3);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled5, 0, 4);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled6, 0, 5);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled7, 0, 6);
            this->tableLayoutPanel1->Controls->Add(this->threadEnabled8, 0, 7);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount8, 2, 7);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount1, 2, 0);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount2, 2, 1);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount3, 2, 2);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount4, 2, 3);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount5, 2, 4);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount6, 2, 5);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobCount7, 2, 6);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar1, 1, 0);
            this->tableLayoutPanel1->Controls->Add(this->threadMicroJobsBar2, 1, 1);
            this->tableLayoutPanel1->Location = System::Drawing::Point(58, 71);
            this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
            this->tableLayoutPanel1->RowCount = 8;
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
            this->tableLayoutPanel1->Size = System::Drawing::Size(257, 200);
            this->tableLayoutPanel1->TabIndex = 18;
            this->tableLayoutPanel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::tableLayoutPanel1_Paint);
            // 
            // threadMicroJobsBar3
            // 
            this->threadMicroJobsBar3->Location = System::Drawing::Point(56, 54);
            this->threadMicroJobsBar3->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar3->Name = L"threadMicroJobsBar3";
            this->threadMicroJobsBar3->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar3->TabIndex = 17;
            this->threadMicroJobsBar3->Value = 50;
            // 
            // threadMicroJobsBar4
            // 
            this->threadMicroJobsBar4->Location = System::Drawing::Point(56, 79);
            this->threadMicroJobsBar4->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar4->Name = L"threadMicroJobsBar4";
            this->threadMicroJobsBar4->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar4->TabIndex = 17;
            this->threadMicroJobsBar4->Value = 50;
            // 
            // threadMicroJobsBar5
            // 
            this->threadMicroJobsBar5->Location = System::Drawing::Point(56, 104);
            this->threadMicroJobsBar5->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar5->Name = L"threadMicroJobsBar5";
            this->threadMicroJobsBar5->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar5->TabIndex = 17;
            this->threadMicroJobsBar5->Value = 50;
            // 
            // threadMicroJobsBar6
            // 
            this->threadMicroJobsBar6->Location = System::Drawing::Point(56, 129);
            this->threadMicroJobsBar6->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar6->Name = L"threadMicroJobsBar6";
            this->threadMicroJobsBar6->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar6->TabIndex = 17;
            this->threadMicroJobsBar6->Value = 50;
            // 
            // threadMicroJobsBar7
            // 
            this->threadMicroJobsBar7->Location = System::Drawing::Point(56, 154);
            this->threadMicroJobsBar7->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar7->Name = L"threadMicroJobsBar7";
            this->threadMicroJobsBar7->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar7->TabIndex = 17;
            this->threadMicroJobsBar7->Value = 50;
            // 
            // threadMicroJobsBar8
            // 
            this->threadMicroJobsBar8->Location = System::Drawing::Point(56, 179);
            this->threadMicroJobsBar8->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar8->Name = L"threadMicroJobsBar8";
            this->threadMicroJobsBar8->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar8->TabIndex = 17;
            this->threadMicroJobsBar8->Value = 50;
            // 
            // threadEnabled1
            // 
            this->threadEnabled1->AutoSize = true;
            this->threadEnabled1->Location = System::Drawing::Point(3, 4);
            this->threadEnabled1->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled1->Name = L"threadEnabled1";
            this->threadEnabled1->Size = System::Drawing::Size(37, 17);
            this->threadEnabled1->TabIndex = 18;
            this->threadEnabled1->Text = L"#1";
            this->threadEnabled1->UseVisualStyleBackColor = true;
            // 
            // threadEnabled2
            // 
            this->threadEnabled2->AutoSize = true;
            this->threadEnabled2->Location = System::Drawing::Point(3, 29);
            this->threadEnabled2->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled2->Name = L"threadEnabled2";
            this->threadEnabled2->Size = System::Drawing::Size(37, 17);
            this->threadEnabled2->TabIndex = 18;
            this->threadEnabled2->Text = L"#2";
            this->threadEnabled2->UseVisualStyleBackColor = true;
            // 
            // threadEnabled3
            // 
            this->threadEnabled3->AutoSize = true;
            this->threadEnabled3->Location = System::Drawing::Point(3, 54);
            this->threadEnabled3->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled3->Name = L"threadEnabled3";
            this->threadEnabled3->Size = System::Drawing::Size(37, 17);
            this->threadEnabled3->TabIndex = 18;
            this->threadEnabled3->Text = L"#3";
            this->threadEnabled3->UseVisualStyleBackColor = true;
            // 
            // threadEnabled4
            // 
            this->threadEnabled4->AutoSize = true;
            this->threadEnabled4->Location = System::Drawing::Point(3, 79);
            this->threadEnabled4->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled4->Name = L"threadEnabled4";
            this->threadEnabled4->Size = System::Drawing::Size(37, 17);
            this->threadEnabled4->TabIndex = 18;
            this->threadEnabled4->Text = L"#4";
            this->threadEnabled4->UseVisualStyleBackColor = true;
            // 
            // threadEnabled5
            // 
            this->threadEnabled5->AutoSize = true;
            this->threadEnabled5->Location = System::Drawing::Point(3, 104);
            this->threadEnabled5->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled5->Name = L"threadEnabled5";
            this->threadEnabled5->Size = System::Drawing::Size(37, 17);
            this->threadEnabled5->TabIndex = 18;
            this->threadEnabled5->Text = L"#5";
            this->threadEnabled5->UseVisualStyleBackColor = true;
            // 
            // threadEnabled6
            // 
            this->threadEnabled6->AutoSize = true;
            this->threadEnabled6->Location = System::Drawing::Point(3, 129);
            this->threadEnabled6->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled6->Name = L"threadEnabled6";
            this->threadEnabled6->Size = System::Drawing::Size(37, 17);
            this->threadEnabled6->TabIndex = 18;
            this->threadEnabled6->Text = L"#6";
            this->threadEnabled6->UseVisualStyleBackColor = true;
            // 
            // threadEnabled7
            // 
            this->threadEnabled7->AutoSize = true;
            this->threadEnabled7->Location = System::Drawing::Point(3, 154);
            this->threadEnabled7->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled7->Name = L"threadEnabled7";
            this->threadEnabled7->Size = System::Drawing::Size(37, 17);
            this->threadEnabled7->TabIndex = 18;
            this->threadEnabled7->Text = L"#7";
            this->threadEnabled7->UseVisualStyleBackColor = true;
            // 
            // threadEnabled8
            // 
            this->threadEnabled8->AutoSize = true;
            this->threadEnabled8->Location = System::Drawing::Point(3, 179);
            this->threadEnabled8->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadEnabled8->Name = L"threadEnabled8";
            this->threadEnabled8->Size = System::Drawing::Size(37, 17);
            this->threadEnabled8->TabIndex = 18;
            this->threadEnabled8->Text = L"#8";
            this->threadEnabled8->UseVisualStyleBackColor = true;
            // 
            // threadMicroJobCount8
            // 
            this->threadMicroJobCount8->Location = System::Drawing::Point(212, 175);
            this->threadMicroJobCount8->Name = L"threadMicroJobCount8";
            this->threadMicroJobCount8->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount8->TabIndex = 19;
            this->threadMicroJobCount8->Text = L"0";
            this->threadMicroJobCount8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount1
            // 
            this->threadMicroJobCount1->Location = System::Drawing::Point(212, 0);
            this->threadMicroJobCount1->Name = L"threadMicroJobCount1";
            this->threadMicroJobCount1->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount1->TabIndex = 19;
            this->threadMicroJobCount1->Text = L"0";
            this->threadMicroJobCount1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount2
            // 
            this->threadMicroJobCount2->Location = System::Drawing::Point(212, 25);
            this->threadMicroJobCount2->Name = L"threadMicroJobCount2";
            this->threadMicroJobCount2->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount2->TabIndex = 19;
            this->threadMicroJobCount2->Text = L"0";
            this->threadMicroJobCount2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount3
            // 
            this->threadMicroJobCount3->Location = System::Drawing::Point(212, 50);
            this->threadMicroJobCount3->Name = L"threadMicroJobCount3";
            this->threadMicroJobCount3->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount3->TabIndex = 19;
            this->threadMicroJobCount3->Text = L"0";
            this->threadMicroJobCount3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount4
            // 
            this->threadMicroJobCount4->Location = System::Drawing::Point(212, 75);
            this->threadMicroJobCount4->Name = L"threadMicroJobCount4";
            this->threadMicroJobCount4->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount4->TabIndex = 19;
            this->threadMicroJobCount4->Text = L"0";
            this->threadMicroJobCount4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount5
            // 
            this->threadMicroJobCount5->Location = System::Drawing::Point(212, 100);
            this->threadMicroJobCount5->Name = L"threadMicroJobCount5";
            this->threadMicroJobCount5->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount5->TabIndex = 19;
            this->threadMicroJobCount5->Text = L"0";
            this->threadMicroJobCount5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            this->threadMicroJobCount5->Click += gcnew System::EventHandler(this, &Form1::label13_Click);
            // 
            // threadMicroJobCount6
            // 
            this->threadMicroJobCount6->Location = System::Drawing::Point(212, 125);
            this->threadMicroJobCount6->Name = L"threadMicroJobCount6";
            this->threadMicroJobCount6->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount6->TabIndex = 19;
            this->threadMicroJobCount6->Text = L"0";
            this->threadMicroJobCount6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // threadMicroJobCount7
            // 
            this->threadMicroJobCount7->ImeMode = System::Windows::Forms::ImeMode::NoControl;
            this->threadMicroJobCount7->Location = System::Drawing::Point(212, 150);
            this->threadMicroJobCount7->Name = L"threadMicroJobCount7";
            this->threadMicroJobCount7->Size = System::Drawing::Size(39, 19);
            this->threadMicroJobCount7->TabIndex = 19;
            this->threadMicroJobCount7->Text = L"0";
            this->threadMicroJobCount7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            this->threadMicroJobCount7->Click += gcnew System::EventHandler(this, &Form1::label15_Click);
            // 
            // threadMicroJobsBar2
            // 
            this->threadMicroJobsBar2->Location = System::Drawing::Point(56, 29);
            this->threadMicroJobsBar2->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->threadMicroJobsBar2->Name = L"threadMicroJobsBar2";
            this->threadMicroJobsBar2->Size = System::Drawing::Size(150, 17);
            this->threadMicroJobsBar2->TabIndex = 17;
            this->threadMicroJobsBar2->Value = 50;
            // 
            // label10
            // 
            this->label10->AutoSize = true;
            this->label10->Location = System::Drawing::Point(6, 71);
            this->label10->Name = L"label10";
            this->label10->Size = System::Drawing::Size(45, 13);
            this->label10->TabIndex = 19;
            this->label10->Text = L"Threads";
            this->label10->Click += gcnew System::EventHandler(this, &Form1::label10_Click);
            // 
            // label6
            // 
            this->label6->AutoSize = true;
            this->label6->Location = System::Drawing::Point(6, 283);
            this->label6->Name = L"label6";
            this->label6->Size = System::Drawing::Size(67, 13);
            this->label6->TabIndex = 21;
            this->label6->Text = L"Image Cache";
            this->label6->Click += gcnew System::EventHandler(this, &Form1::label6_Click_1);
            // 
            // label8
            // 
            this->label8->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->label8->Location = System::Drawing::Point(79, 282);
            this->label8->Name = L"label8";
            this->label8->Size = System::Drawing::Size(67, 14);
            this->label8->TabIndex = 12;
            this->label8->Text = L"0";
            this->label8->TextAlign = System::Drawing::ContentAlignment::TopRight;
            this->label8->Click += gcnew System::EventHandler(this, &Form1::jobCountH_Click);
            // 
            // imageCacheClearButton
            // 
            this->imageCacheClearButton->Location = System::Drawing::Point(152, 278);
            this->imageCacheClearButton->Name = L"imageCacheClearButton";
            this->imageCacheClearButton->Size = System::Drawing::Size(75, 23);
            this->imageCacheClearButton->TabIndex = 22;
            this->imageCacheClearButton->Text = L"Clear";
            this->imageCacheClearButton->UseVisualStyleBackColor = true;
            this->imageCacheClearButton->Click += gcnew System::EventHandler(this, &Form1::imageCacheClearButton_Click);
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(104, 12);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(43, 23);
            this->button1->TabIndex = 23;
            this->button1->Text = L"Apply";
            this->button1->UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this->label4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->label4->Location = System::Drawing::Point(152, 12);
            this->label4->Name = L"label4";
            this->label4->Size = System::Drawing::Size(163, 23);
            this->label4->TabIndex = 24;
            this->label4->Text = L"Not Connected";
            this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(324, 313);
            this->Controls->Add(this->label4);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->imageCacheClearButton);
            this->Controls->Add(this->label6);
            this->Controls->Add(this->label10);
            this->Controls->Add(this->tableLayoutPanel1);
            this->Controls->Add(this->jobCountL);
            this->Controls->Add(this->label7);
            this->Controls->Add(this->jobCountM);
            this->Controls->Add(this->label5);
            this->Controls->Add(this->label8);
            this->Controls->Add(this->jobCountH);
            this->Controls->Add(this->label3);
            this->Controls->Add(this->label2);
            this->Controls->Add(this->portNumberBox);
            this->Controls->Add(this->label1);
            this->Font = (gcnew System::Drawing::Font(L"Calibri", 8));
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
            this->Margin = System::Windows::Forms::Padding(3, 4, 3, 4);
            this->Name = L"Form1";
            this->Text = L"Garnet Control Panel";
            this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
            this->tableLayoutPanel1->ResumeLayout(false);
            this->tableLayoutPanel1->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
             }
    private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
             }
    private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
             }
    private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
             }
private: System::Void label8_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label7_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label6_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label5_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void EnableThread0_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label12_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void tableLayoutPanel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
         }
private: System::Void label13_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label15_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void jobCountH_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label10_Click(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void label6_Click_1(System::Object^  sender, System::EventArgs^  e) {
         }
private: System::Void imageCacheClearButton_Click(System::Object^  sender, System::EventArgs^  e) {
         }
};
}

